from __future__ import annotations

import json
import os
import re
import tarfile
import zipfile
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Tuple

# ---------- File discovery ----------
MASTER_PATTERNS = (r"(?i)master\.log", r"(?i)ecmaster", r"(?i)\bmaster\b")
AMC_RECV_PATTERNS = (r"(?i)amc.*recv", r"(?i)recv.*amc", r"(?i)amc[_-]?recv")
TRACE_PATTERNS = (r"(?i)M[-_]?TRACE", r"(?i)C[-_]?TRACE", r"(?i)\bTRACE\b")
USER_PATTERNS = (r"(?i)user.*\.log", r"(?i)\buser\b", r"(?i)\bui.*\.log")

STRICT_ERR_PAT = re.compile(r"(\bERROR\b|\bFAULT\b|\bALARM\b|ALM\b|0x[0-9A-Fa-f]{2,8})")


def _is_texty(name: str) -> bool:
    bad_ext = (
        ".png",
        ".jpg",
        ".jpeg",
        ".bmp",
        ".gif",
        ".zip",
        ".7z",
        ".tar",
        ".gz",
        ".xz",
        ".dll",
        ".so",
        ".bin",
        ".exe",
    )
    return not name.lower().endswith(bad_ext)


def _match_any(name: str, pats: Iterable[str]) -> bool:
    low = name.lower()
    return any(re.search(p, low) for p in pats)


def _iter_bundle(bundle: Path) -> Iterable[Tuple[str, bytes]]:
    """Yield (virtual_path, bytes) for each file within folder/zip/tar/single."""
    if bundle.is_dir():
        for fp in bundle.rglob("*"):
            if fp.is_file():
                yield (str(fp), fp.read_bytes())
        return
    lower = bundle.name.lower()
    if lower.endswith(".zip"):
        with zipfile.ZipFile(bundle, "r") as zp:
            for n in zp.namelist():
                if _is_texty(n):
                    yield (n, zp.read(n))
        return
    if lower.endswith((".tar", ".tar.gz", ".tgz", ".tar.bz2", ".tbz2", ".tar.xz", ".txz")):
        with tarfile.open(bundle, "r:*") as tp:
            for m in tp.getmembers():
                if m.isfile():
                    f = tp.extractfile(m)
                    if f:
                        yield (m.name, f.read())
        return
    if bundle.is_file():
        yield (bundle.name, bundle.read_bytes())


# ---------- Small utilities ----------
def _read_text_guess(b: bytes) -> str:
    for enc in ("utf-8-sig", "utf-8", "cp949", "euc-kr", "latin-1"):
        try:
            return b.decode(enc)
        except Exception:
            continue
    return b.decode("latin-1", errors="ignore")


def _grep_errors(text: str, max_hits: int = 500) -> List[Dict]:
    out: List[Dict] = []
    for i, ln in enumerate(text.splitlines(), start=1):
        if STRICT_ERR_PAT.search(ln):
            out.append({"line": i, "text": ln.strip()})
            if len(out) >= max_hits:
                break
    return out


def _filter_axis(
    lines: List[Dict], axis: int = 3, keywords: Tuple[str, ...] = ("slide", "슬라이드")
) -> List[Dict]:
    pats = [rf"axis\s*{axis}", rf"axis\[{axis}\]"] + list(keywords)
    sel = []
    for row in lines:
        t = row["text"]
        if any(re.search(p, t, re.IGNORECASE) for p in pats):
            sel.append(row)
    return sel


# ---------- Trace invocation (optional) ----------
def _try_plot_traces(bundle: Path, out_dir: Path) -> List[Dict]:
    """
    If mtrace_visualizer is installed in the repo, call it.
    Otherwise, just list found TRACE files in order.
    """
    results: List[Dict] = []
    try:
        # Lazy import (may not exist yet in some deployments)
        from oht_analyzer.vis.mtrace_visualizer import visualize_from_bundle

        vis = visualize_from_bundle(
            str(bundle), str(out_dir), smooth_window=5, max_points=120_000
        )
        for v in vis:
            results.append(
                {
                    "source": v.source,
                    "speed_png": str(v.speed_png)
                    if getattr(v, "speed_png", None)
                    else None,
                    "pos_png": str(v.pos_png) if getattr(v, "pos_png", None) else None,
                    "notes": v.notes,
                }
            )
        return results
    except Exception:
        pass

    # fallback: just enumerate TRACE files so UI can link them
    for vp, _ in _iter_bundle(bundle):
        if _match_any(vp, TRACE_PATTERNS):
            results.append(
                {
                    "source": vp,
                    "speed_png": None,
                    "pos_png": None,
                    "notes": "visualizer not available",
                }
            )
    return results


# ---------- Main API ----------
@dataclass
class AnalysisResult:
    plan: List[Dict]
    artifacts_dir: str


def analyze_in_order(
    bundle_path: str | os.PathLike, out_dir: str | os.PathLike, axis_focus: int = 3
) -> AnalysisResult:
    """
    Execute the **standard analysis order**:
      1) Check master logs
      2) If present, summarize master errors first
      3) Summarize AMC Recv logs
      4) Plot/collect TRACE logs (M-TRACE/C-TRACE)
      5) Summarize USER logs
      6) (Reserved) later: cross-log reasoning (not included here)
    Returns a plan + artifact pointers for UI rendering.
    """
    bundle = Path(bundle_path)
    outroot = Path(out_dir)
    outroot.mkdir(parents=True, exist_ok=True)

    master_items: List[Dict] = []
    amc_items: List[Dict] = []
    trace_items: List[Dict] = []
    user_items: List[Dict] = []

    # Discover all
    files: List[Tuple[str, bytes]] = list(_iter_bundle(bundle))
    # 1,2) Master first
    for vp, data in files:
        if _match_any(vp, MASTER_PATTERNS) and _is_texty(vp):
            txt = _read_text_guess(data)
            errs = _grep_errors(txt)
            axis_errs = _filter_axis(errs, axis=axis_focus)
            master_items.append(
                {
                    "file": vp,
                    "errors_total": len(errs),
                    "axis_focus_errors": axis_errs[:200],
                }
            )
    # 3) AMC Recv
    for vp, data in files:
        if _match_any(vp, AMC_RECV_PATTERNS) and _is_texty(vp):
            txt = _read_text_guess(data)
            hits = _grep_errors(txt)
            amc_items.append({"file": vp, "errors": hits[:200]})
    # 4) TRACE
    trace_dir = outroot / "trace_plots"
    trace_dir.mkdir(parents=True, exist_ok=True)
    trace_items = _try_plot_traces(bundle, trace_dir)
    # 5) USER
    for vp, data in files:
        if _match_any(vp, USER_PATTERNS) and _is_texty(vp):
            txt = _read_text_guess(data)
            hits = _grep_errors(txt)
            user_items.append({"file": vp, "errors": hits[:200]})

    # Assemble plan in the prescribed order
    plan: List[Dict] = []
    if master_items:
        plan.append({"step": 1, "title": "Master logs (first)", "items": master_items})
    plan.append({"step": 2, "title": "AMC Recv logs", "items": amc_items})
    plan.append({"step": 3, "title": "Trace logs (M-TRACE/C-TRACE)", "items": trace_items})
    plan.append({"step": 4, "title": "User logs", "items": user_items})
    plan.append({"step": 5, "title": "Cross-log reasoning (reserved)", "items": []})

    # Save a machine-readable summary for UI
    summary_path = outroot / "analysis_plan.json"
    summary_path.write_text(
        json.dumps({"plan": plan}, ensure_ascii=False, indent=2), encoding="utf-8"
    )

    return AnalysisResult(plan=plan, artifacts_dir=str(outroot))

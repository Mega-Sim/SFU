from __future__ import annotations
from typing import Dict, Any, Iterable, Optional, Set
from collections import defaultdict

from .parser import iter_logs, find_time_ms
from .rules import RuleSet
from . import storage

CYCLE_MS = 1


def assert_required_sources() -> None:
    if not storage.required_sources_present(("vehicle", "motion")):
        raise RuntimeError(
            "vehicle_control.zip과 motion_control.zip을 모두 인덱싱한 뒤에만 분석을 진행할 수 있습니다."
        )


def _normalize_target_codes(codes: Optional[Iterable[str]]) -> Optional[Set[str]]:
    if not codes:
        return None

    normalized: Set[str] = set()
    for raw in codes:
        if raw is None:
            continue
        text = str(raw).strip()
        if not text:
            continue
        if text.upper().startswith("E"):
            text = text[1:]
        if text:
            normalized.add(text)
    return normalized or None


def analyze(paths, rules: RuleSet, target_codes: Optional[Iterable[str]] = None) -> Dict[str, Any]:
    assert_required_sources()
    code_filter = _normalize_target_codes(target_codes)
    lines = []
    by_cat = defaultdict(list)
    for fname, text in iter_logs(paths):
        cat = rules.categorize(fname)
        for raw in text.splitlines():
            ts = find_time_ms(raw)
            rec = {"file": fname, "cat": cat, "ts": ts, "text": raw}
            lines.append(rec); by_cat[cat].append(rec)

    anchors = []
    for rec in lines:
        for _, code in rules.match_anchors(rec["text"]):
            if rec["ts"] is None:
                continue
            code_str = str(code)
            if code_filter and code_str not in code_filter:
                continue
            anchors.append({"code": code_str, "ts": rec["ts"], "file": rec["file"], "text": rec["text"]})
    anchors.sort(key=lambda x: x["ts"])

    wnd = rules.windows
    per_code = defaultdict(list)
    for a in anchors:
        per_code[a["code"]].append(a["ts"])
    code_windows = {}
    for code, tlist in per_code.items():
        merged=[]
        cur=[tlist[0], tlist[0]]
        for t in tlist[1:]:
            if t - cur[1] <= wnd["anchor_merge"]*1000:
                cur[1] = t
            else:
                merged.append(tuple(cur)); cur=[t,t]
        merged.append(tuple(cur))
        code_windows[code] = merged

    precursors = []
    for code, merged in code_windows.items():
        for (start,end) in merged:
            first_anchor = start
            window_start = first_anchor - wnd["precursor_before"]*1000
            window_end   = first_anchor + wnd["precursor_after"]*1000
            for rec in lines:
                ts = rec["ts"]
                if ts is None: continue
                if window_start <= ts <= window_end and rules.is_precursor(rec["text"]):
                    precursors.append({
                        "code": code, "file": rec["file"], "cat": rec["cat"], "ts": ts,
                        "dt_ms": ts - first_anchor, "text": rec["text"]
                    })

    drive_hints = [rec for rec in lines if rec["ts"] is not None and rules.is_drive_hint(rec["text"])]
    drive_samples=[]
    for code, merged in code_windows.items():
        for (start,end) in merged:
            anchor = start
            for rec in drive_hints:
                if anchor-10000 <= rec["ts"] <= anchor+10000:
                    drive_samples.append({"code": code, "file": rec["file"], "ts": rec["ts"], "text": rec["text"]})

    banner = []
    for code, merged in code_windows.items():
        cnt = sum(1 for a in anchors if a["code"]==code)
        b = {
            "code": code, "count": cnt,
            "first": merged[0][0], "last": merged[-1][1],
            "precursor_present": any(p["code"]==code for p in precursors),
            "drive_evidence": any(s["code"]==code for s in drive_samples)
        }
        banner.append(b)

    section = defaultdict(lambda: {"files": set(), "first": None, "last": None, "samples": []})
    def update_sec(cat, rec):
        s = section[cat]
        s["files"].add(rec["file"])
        if rec["ts"] is not None:
            s["first"] = rec["ts"] if s["first"] is None or rec["ts"]<s["first"] else s["first"]
            s["last"]  = rec["ts"] if s["last"] is None or rec["ts"]>s["last"] else s["last"]
    for rec in lines:
        update_sec(rec["cat"], rec)
    for a in anchors[:12]: section[a["file"].split(":")[0]]["samples"].append(a)
    for p in precursors[:12]: section[p["file"].split(":")[0]]["samples"].append(p)

    return {
        "anchors": anchors,
        "code_windows": code_windows,
        "precursors": precursors,
        "drive_samples": drive_samples,
        "banner": banner,
        "section": {k: {"files": list(v["files"]), "first": v["first"], "last": v["last"], "samples": v["samples"]} for k,v in section.items()}
    }

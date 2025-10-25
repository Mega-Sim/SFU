"""Utilities for rendering M-Trace log visualizations.

The main entry point :func:`visualize_from_bundle` inspects a given bundle
(archive/directory/single file), locates M-Trace logs and generates two PNG
charts per file:

* Actual/Command speed vs. torque
* Actual/Command position vs. torque

The helper is resilient to a variety of CSV/LOG encodings, column names and
missing time information.  See the module level constants for the heuristics
that are used when parsing the logs.
"""
from __future__ import annotations

import argparse
import io
import json
import math
import re
import tarfile
import zipfile
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, Iterator, List, Optional, Tuple

import numpy as np
import pandas as pd

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


# Regex used to identify M-Trace log files irrespective of extensions.
MTRACE_NAME_REGEX = re.compile(r"(?i)M[-_]?TRACE")

# Common encodings used for M-Trace exports (Korean/English environments).
CANDIDATE_ENCODINGS: Tuple[str, ...] = (
    "utf-8-sig",
    "utf-8",
    "cp949",
    "euc-kr",
    "latin-1",
)

# Token dictionary for fuzzy column matching.
TOKENS: Dict[str, List[str]] = {
    "time": [
        "time",
        "timestamp",
        "t",
        "ms",
        "time_ms",
        "tick",
        "sample",
        "index",
        "번호",
        "시간",
        "타임",
        "타임스탬프",
    ],
    "torque": [
        "torque",
        "trq",
        "iq",
        "iqref",
        "iqfb",
        "current",
        "load",
        "motor_torque",
        "토크",
        "토오크",
        "전류",
        "부하",
    ],
    "speed_act": [
        "actualspeed",
        "actspeed",
        "speed",
        "velocity",
        "actvel",
        "feedbackspeed",
        "fbvel",
        "measvel",
        "velfb",
        "실제속도",
        "피드백속도",
        "피드백속력",
    ],
    "speed_cmd": [
        "commandspeed",
        "cmdspeed",
        "targetspeed",
        "refspeed",
        "setpointspeed",
        "velcmd",
        "vref",
        "명령속도",
        "지령속도",
        "설정속도",
    ],
    "pos_act": [
        "actualposition",
        "actposition",
        "position",
        "actpos",
        "feedbackposition",
        "fbpos",
        "measpos",
        "posfb",
        "실제위치",
        "피드백위치",
    ],
    "pos_cmd": [
        "commandposition",
        "cmdposition",
        "targetposition",
        "refposition",
        "setpointposition",
        "cmdpos",
        "poscmd",
        "명령위치",
        "지령위치",
        "설정위치",
    ],
}


def _normalize(text: str) -> str:
    """Return a simplified identifier used for fuzzy comparisons."""

    return re.sub(r"[^a-z0-9가-힣]+", "", text.lower())


def _find_best_column(columns: Iterable[str], token_list: Iterable[str]) -> Optional[str]:
    """Find the column whose normalised name best matches the token list."""

    columns = list(columns)
    normalised = {column: _normalize(column) for column in columns}
    candidates = [_normalize(token) for token in token_list]

    # Prefer exact/starts-with matches.
    for column, normalised_column in normalised.items():
        for candidate in candidates:
            if normalised_column == candidate or normalised_column.startswith(candidate):
                return column

    # Fallback to substring matches.
    for column, normalised_column in normalised.items():
        for candidate in candidates:
            if candidate and candidate in normalised_column:
                return column

    return None


def _read_csv_like(buffer: io.BytesIO, source_name: str) -> pd.DataFrame:
    """Attempt to read a CSV/LOG buffer with multiple encodings and separators."""

    last_error: Optional[Exception] = None
    for encoding in CANDIDATE_ENCODINGS:
        try:
            buffer.seek(0)
            df = pd.read_csv(buffer, sep=None, engine="python", encoding=encoding)
            if df.shape[1] == 1:
                buffer.seek(0)
                df = pd.read_csv(
                    buffer,
                    delim_whitespace=True,
                    engine="python",
                    encoding=encoding,
                )
            return df
        except Exception as exc:  # pragma: no cover - pandas raises various errors.
            last_error = exc
    raise RuntimeError(f"Failed to parse {source_name}. Last error: {last_error!r}")


def _build_time_axis(df: pd.DataFrame, time_column: Optional[str]) -> pd.Series:
    """Return a time axis in seconds for the dataframe."""

    if time_column and time_column in df.columns:
        series = df[time_column]
        numeric = pd.to_numeric(series, errors="coerce")
        if numeric.notna().mean() > 0.9:
            if not numeric.dropna().empty:
                median = float(numeric.dropna().median())
                # Treat high values as millisecond ticks.
                if 1000.0 < median < 1e9:
                    return numeric / 1000.0
            return numeric
        try:
            parsed = pd.to_datetime(series)
            return (parsed - parsed.iloc[0]).dt.total_seconds()
        except Exception:  # pragma: no cover - depends on input formats.
            pass

    return pd.Series(np.arange(len(df)) * 0.001, name="time_s")


def _smooth(series: pd.Series, window: int) -> pd.Series:
    if window and window > 1:
        return series.rolling(window, min_periods=1, center=False).mean()
    return series


def _downsample_df(df: pd.DataFrame, max_points: int) -> pd.DataFrame:
    if len(df) <= max_points:
        return df
    step = max(1, math.ceil(len(df) / max_points))
    return df.iloc[::step, :].reset_index(drop=True)


def _parse_columns(df: pd.DataFrame) -> Dict[str, Optional[str]]:
    columns = list(df.columns.astype(str))
    return {
        "time": _find_best_column(columns, TOKENS["time"]),
        "torque": _find_best_column(columns, TOKENS["torque"]),
        "speed_act": _find_best_column(columns, TOKENS["speed_act"]),
        "speed_cmd": _find_best_column(columns, TOKENS["speed_cmd"]),
        "pos_act": _find_best_column(columns, TOKENS["pos_act"]),
        "pos_cmd": _find_best_column(columns, TOKENS["pos_cmd"]),
    }


def _plot_speed_vs_torque(
    df: pd.DataFrame,
    time_s: pd.Series,
    actual: str,
    command: Optional[str],
    torque: str,
    output_path: Path,
) -> None:
    fig = plt.figure(figsize=(12, 5))
    ax_left = fig.add_subplot(111)
    ax_left.plot(time_s, df[actual], label="Actual Speed")
    if command and command in df:
        ax_left.plot(time_s, df[command], label="Command Speed")
    ax_left.set_xlabel("Time (s)")
    ax_left.set_ylabel("Speed")
    ax_left.legend(loc="upper left")

    ax_right = ax_left.twinx()
    ax_right.plot(time_s, df[torque], label="Torque", alpha=0.85)
    ax_right.set_ylabel("Torque")

    fig.suptitle("Speed vs Torque (left: speed, right: torque)")
    fig.tight_layout()
    fig.savefig(output_path, dpi=150)
    plt.close(fig)


def _plot_position_vs_torque(
    df: pd.DataFrame,
    time_s: pd.Series,
    actual: str,
    command: Optional[str],
    torque: str,
    output_path: Path,
) -> None:
    fig = plt.figure(figsize=(12, 5))
    ax_left = fig.add_subplot(111)
    ax_left.plot(time_s, df[actual], label="Actual Position")
    if command and command in df:
        ax_left.plot(time_s, df[command], label="Command Position")
    ax_left.set_xlabel("Time (s)")
    ax_left.set_ylabel("Position")
    ax_left.legend(loc="upper left")

    ax_right = ax_left.twinx()
    ax_right.plot(time_s, df[torque], label="Torque", alpha=0.85)
    ax_right.set_ylabel("Torque")

    fig.suptitle("Position vs Torque (left: position, right: torque)")
    fig.tight_layout()
    fig.savefig(output_path, dpi=150)
    plt.close(fig)


def _iter_mtrace_files_from_dir(root: Path) -> Iterator[Tuple[str, io.BytesIO]]:
    for path in root.rglob("*"):
        if not path.is_file():
            continue
        if not MTRACE_NAME_REGEX.search(path.name):
            continue
        try:
            yield str(path), io.BytesIO(path.read_bytes())
        except OSError:
            continue


def _iter_mtrace_files_from_zip(archive: zipfile.ZipFile, zip_path: Path) -> Iterator[Tuple[str, io.BytesIO]]:
    for name in archive.namelist():
        if name.endswith("/"):
            continue
        base = Path(name).name
        if MTRACE_NAME_REGEX.search(base):
            yield f"{zip_path}!{name}", io.BytesIO(archive.read(name))


def _iter_mtrace_files_from_tar(archive: tarfile.TarFile, tar_path: Path) -> Iterator[Tuple[str, io.BytesIO]]:
    for member in archive.getmembers():
        if not member.isfile():
            continue
        base = Path(member.name).name
        if not MTRACE_NAME_REGEX.search(base):
            continue
        extracted = archive.extractfile(member)
        if extracted is None:
            continue
        yield f"{tar_path}!{member.name}", io.BytesIO(extracted.read())


def _iter_mtrace_streams(bundle: Path) -> Iterator[Tuple[str, io.BytesIO]]:
    if bundle.is_dir():
        yield from _iter_mtrace_files_from_dir(bundle)
        return

    lower = bundle.name.lower()
    if lower.endswith(".zip"):
        with zipfile.ZipFile(bundle, "r") as archive:
            yield from _iter_mtrace_files_from_zip(archive, bundle)
        return

    if lower.endswith((".tar", ".tar.gz", ".tgz", ".tar.bz2", ".tbz2", ".tar.xz", ".txz")):
        with tarfile.open(bundle, "r:*") as archive:
            yield from _iter_mtrace_files_from_tar(archive, bundle)
        return

    if MTRACE_NAME_REGEX.search(bundle.name):
        yield str(bundle), io.BytesIO(bundle.read_bytes())


@dataclass
class VisualizeResult:
    source: str
    speed_png: Optional[Path]
    pos_png: Optional[Path]
    notes: str


def visualize_from_bundle(
    bundle_path: Path | str,
    out_dir: Path | str,
    *,
    smooth_window: int = 1,
    max_points: int = 100_000,
) -> List[VisualizeResult]:
    """Generate plots for any M-Trace files contained in ``bundle_path``."""

    bundle = Path(bundle_path)
    output_root = Path(out_dir)
    output_root.mkdir(parents=True, exist_ok=True)

    results: List[VisualizeResult] = []
    found = False

    for source, stream in _iter_mtrace_streams(bundle):
        found = True
        notes: List[str] = []
        try:
            raw_bytes = stream.getvalue()

            def reopen() -> io.BytesIO:
                return io.BytesIO(raw_bytes)

            df = _read_csv_like(reopen(), source)
            column_map = _parse_columns(df)

            keep_columns = [
                column_map.get("time"),
                column_map.get("torque"),
                column_map.get("speed_act"),
                column_map.get("speed_cmd"),
                column_map.get("pos_act"),
                column_map.get("pos_cmd"),
            ]
            keep_columns = [col for col in keep_columns if col]
            if not keep_columns:
                results.append(
                    VisualizeResult(
                        source=source,
                        speed_png=None,
                        pos_png=None,
                        notes="No recognizable columns; headers: "
                        + ", ".join(df.columns.astype(str)),
                    )
                )
                continue

            df = df.loc[:, keep_columns].copy()
            for key in ("torque", "speed_act", "speed_cmd", "pos_act", "pos_cmd"):
                column = column_map.get(key)
                if column and column in df.columns:
                    df[column] = _smooth(pd.to_numeric(df[column], errors="coerce"), smooth_window)

            time_series = _build_time_axis(df, column_map.get("time"))

            combined = pd.DataFrame({"__time__": time_series})
            for column in df.columns:
                combined[column] = df[column]

            combined = _downsample_df(combined, max_points)
            time_series = combined["__time__"]
            df = combined.drop(columns=["__time__"])

            safe_name = re.sub(r"[^a-zA-Z0-9._-]+", "_", Path(source).name)
            base_path = output_root / safe_name

            speed_path: Optional[Path] = None
            position_path: Optional[Path] = None

            torque_column = column_map.get("torque")
            if torque_column and column_map.get("speed_act"):
                speed_path = base_path.with_name(base_path.stem + "_speed_vs_torque.png")
                _plot_speed_vs_torque(
                    df,
                    time_series,
                    column_map["speed_act"],
                    column_map.get("speed_cmd"),
                    torque_column,
                    speed_path,
                )

            if torque_column and column_map.get("pos_act"):
                position_path = base_path.with_name(base_path.stem + "_position_vs_torque.png")
                _plot_position_vs_torque(
                    df,
                    time_series,
                    column_map["pos_act"],
                    column_map.get("pos_cmd"),
                    torque_column,
                    position_path,
                )

            if speed_path is None and position_path is None:
                notes.append(f"Missing columns for plots. mapping={column_map}")

            results.append(
                VisualizeResult(
                    source=source,
                    speed_png=speed_path,
                    pos_png=position_path,
                    notes="; ".join(notes),
                )
            )
        except Exception as exc:  # pragma: no cover - defensive error path.
            results.append(
                VisualizeResult(
                    source=source,
                    speed_png=None,
                    pos_png=None,
                    notes=f"Error: {exc!r}",
                )
            )

    if not found:
        results.append(
            VisualizeResult(
                source=str(bundle),
                speed_png=None,
                pos_png=None,
                notes="No M-Trace files found in bundle.",
            )
        )

    return results


def _main() -> None:
    parser = argparse.ArgumentParser(
        description="M-Trace visualizer (speed/position vs torque)",
    )
    parser.add_argument("--in", dest="inp", required=True, help="Bundle path (zip/tar/dir/file)")
    parser.add_argument("--out", dest="out", required=True, help="Directory to save PNG files")
    parser.add_argument(
        "--smooth",
        dest="smooth",
        type=int,
        default=1,
        help="Rolling window size for smoothing (in samples)",
    )
    parser.add_argument(
        "--max-points",
        dest="max_points",
        type=int,
        default=100_000,
        help="Maximum number of samples per plot after downsampling",
    )
    args = parser.parse_args()

    results = visualize_from_bundle(
        args.inp,
        args.out,
        smooth_window=args.smooth,
        max_points=args.max_points,
    )

    def to_dict(value: VisualizeResult) -> Dict[str, Optional[str]]:
        return {
            "source": value.source,
            "speed_png": str(value.speed_png) if value.speed_png else None,
            "pos_png": str(value.pos_png) if value.pos_png else None,
            "notes": value.notes,
        }

    print(json.dumps([to_dict(result) for result in results], ensure_ascii=False, indent=2))


if __name__ == "__main__":  # pragma: no cover - CLI entry point.
    _main()

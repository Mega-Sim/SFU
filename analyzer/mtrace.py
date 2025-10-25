"""M-Trace parser & visualizer for the SFU Streamlit app."""
from __future__ import annotations

import io
import math
import re
from pathlib import Path
from typing import Iterable, List, Optional

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


FILE_NAME_PATTERNS = [
    "*M_TRACE*.csv",
    "*M-TRACE*.csv",
    "*MTRACE*.csv",
    "*M_TRACE*.log",
    "*M-TRACE*.log",
    "*MTRACE*.log",
    "AMC_AXIS_M_TRACE*.csv",
    "AMC_AXIS_M_TRACE*.log",
]

CANDIDATE_ENCODINGS = ["utf-8-sig", "utf-8", "cp949", "euc-kr", "latin-1"]


def _norm(text: str) -> str:
    return re.sub(r"[^a-z0-9가-힣]", "", text.lower())


TOKENS = {
    "time": [
        "time",
        "timestamp",
        "t",
        "ms",
        "time_ms",
        "tick",
        "sample",
        "시간",
        "타임",
        "타임스탬프",
    ],
    "torque": [
        "torque",
        "trq",
        "iq",
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
        "명령위치",
        "지령위치",
        "설정위치",
    ],
}


def _find_best(columns: List[str], token_list: List[str]) -> Optional[str]:
    normalized = {col: _norm(col) for col in columns}
    tokens = [_norm(token) for token in token_list]
    for col, norm_col in normalized.items():
        for token in tokens:
            if norm_col == token or norm_col.startswith(token):
                return col
    for col, norm_col in normalized.items():
        for token in tokens:
            if token in norm_col:
                return col
    return None


def _read_any_delim_bytes(raw: bytes) -> pd.DataFrame:
    last_err: Exception | None = None
    for enc in CANDIDATE_ENCODINGS:
        try:
            buffer = io.BytesIO(raw)
            frame = pd.read_csv(buffer, sep=None, engine="python", encoding=enc)
            if frame.shape[1] == 1:
                buffer = io.BytesIO(raw)
                frame = pd.read_csv(buffer, delim_whitespace=True, engine="python", encoding=enc)
            return frame
        except Exception as exc:  # pragma: no cover - best effort sniffing
            last_err = exc
            continue
    raise RuntimeError(
        f"Failed to parse M-Trace with encodings {CANDIDATE_ENCODINGS}. Last error: {last_err}"
    )


def _read_any_delim_path(path: Path) -> pd.DataFrame:
    last_err: Exception | None = None
    for enc in CANDIDATE_ENCODINGS:
        try:
            frame = pd.read_csv(path, sep=None, engine="python", encoding=enc)
            if frame.shape[1] == 1:
                frame = pd.read_csv(path, delim_whitespace=True, engine="python", encoding=enc)
            return frame
        except Exception as exc:  # pragma: no cover - best effort sniffing
            last_err = exc
            continue
    raise RuntimeError(
        f"Failed to parse {path.name} with encodings {CANDIDATE_ENCODINGS}. Last error: {last_err}"
    )


def _mapping(frame: pd.DataFrame) -> dict[str, Optional[str]]:
    columns = list(frame.columns)
    return {
        "time": _find_best(columns, TOKENS["time"]),
        "torque": _find_best(columns, TOKENS["torque"]),
        "speed_act": _find_best(columns, TOKENS["speed_act"]),
        "speed_cmd": _find_best(columns, TOKENS["speed_cmd"]),
        "pos_act": _find_best(columns, TOKENS["pos_act"]),
        "pos_cmd": _find_best(columns, TOKENS["pos_cmd"]),
    }


def _build_time_s(frame: pd.DataFrame, time_col: Optional[str]) -> pd.Series:
    if time_col is not None:
        series = pd.to_numeric(frame[time_col], errors="coerce")
        if series.notna().mean() > 0.9:
            median = series.dropna().median()
            if 1000 < median <= 1_000_000_000:
                return series / 1000.0
            return series
        try:
            timestamps = pd.to_datetime(frame[time_col])
            return (timestamps - timestamps.iloc[0]).dt.total_seconds()
        except Exception:  # pragma: no cover - best effort parsing
            pass
    return pd.Series(np.arange(len(frame)) * 0.001, name="time_s")


def _downsample(frame: pd.DataFrame, max_points: int) -> pd.DataFrame:
    count = len(frame)
    if count <= max_points:
        return frame
    step = math.ceil(count / max_points)
    return frame.iloc[::step, :].reset_index(drop=True)


def _plot_speed_vs_torque(
    frame: pd.DataFrame,
    time_s: pd.Series,
    speed_act: str,
    speed_cmd: Optional[str],
    torque: str,
):
    fig = plt.figure(figsize=(12, 5))
    ax1 = plt.gca()
    ax1.plot(time_s, frame[speed_act], label="Actual Speed")
    if speed_cmd and speed_cmd in frame:
        ax1.plot(time_s, frame[speed_cmd], label="Command Speed")
    ax1.set_xlabel("Time (s)")
    ax1.set_ylabel("Speed")
    ax1.legend(loc="upper left")
    ax2 = ax1.twinx()
    ax2.plot(time_s, frame[torque], label="Torque", alpha=0.85)
    ax2.set_ylabel("Torque")
    plt.title("Speed vs Torque (left: speed, right: torque)")
    return fig


def _plot_pos_vs_torque(
    frame: pd.DataFrame,
    time_s: pd.Series,
    pos_act: str,
    pos_cmd: Optional[str],
    torque: str,
):
    fig = plt.figure(figsize=(12, 5))
    ax1 = plt.gca()
    ax1.plot(time_s, frame[pos_act], label="Actual Position")
    if pos_cmd and pos_cmd in frame:
        ax1.plot(time_s, frame[pos_cmd], label="Command Position")
    ax1.set_xlabel("Time (s)")
    ax1.set_ylabel("Position")
    ax1.legend(loc="upper left")
    ax2 = ax1.twinx()
    ax2.plot(time_s, frame[torque], label="Torque", alpha=0.85)
    ax2.set_ylabel("Torque")
    plt.title("Position vs Torque (left: position, right: torque)")
    return fig


def detect_mtrace_paths(root_dirs: Iterable[Path]) -> List[Path]:
    hits: List[Path] = []
    for root in root_dirs:
        if not root.exists():
            continue
        for pattern in FILE_NAME_PATTERNS:
            hits.extend(root.rglob(pattern))
    hits.sort(key=lambda path: path.stat().st_mtime, reverse=True)
    return hits


def render_mtrace_section(uploaded_files: Optional[List] = None, max_points: int = 100_000) -> None:
    import streamlit as st  # local import to avoid heavy dependency at import time

    st.divider()
    st.subheader("📈 M-Trace Visualization (speed/position vs torque)")

    files = uploaded_files
    if not files:
        files = st.file_uploader(
            "Add M-Trace files (*.csv, *.log). If you already uploaded a big log bundle above, "
            "you can also drop just the M-Trace here.",
            type=["csv", "log"],
            accept_multiple_files=True,
        )

    if not files:
        st.caption("No M-Trace provided yet. Drop files to see charts.")
        return

    for file in files:
        name = getattr(file, "name", None) or str(getattr(file, "path", "M-Trace"))
        st.write(f"**File:** {name}")

        try:
            if hasattr(file, "read"):
                raw = file.read()
                frame = _read_any_delim_bytes(raw)
            elif isinstance(file, Path):
                frame = _read_any_delim_path(file)
            else:
                st.warning("Unsupported file type.")
                continue
        except Exception as exc:  # pragma: no cover - streamlit display path
            st.error(f"Failed to parse: {exc}")
            continue

        mapping = _mapping(frame)
        if not any(mapping.get(key) for key in ("torque", "speed_act", "pos_act")):
            st.warning(f"Columns not recognized. Headers: {list(frame.columns)[:10]}")
            continue

        for key in ["torque", "speed_act", "speed_cmd", "pos_act", "pos_cmd"]:
            column = mapping.get(key)
            if column and column in frame.columns:
                frame[column] = pd.to_numeric(frame[column], errors="coerce")

        time_s = _build_time_s(frame, mapping.get("time"))

        subset = pd.DataFrame({"__t__": time_s})
        for column in {value for value in mapping.values() if value}:
            subset[column] = frame[column]
        subset = _downsample(subset, max_points)
        time_s = subset["__t__"]
        subset = subset.drop(columns=["__t__"])

        if mapping.get("torque") and mapping.get("speed_act"):
            fig_speed = _plot_speed_vs_torque(
                subset,
                time_s,
                mapping["speed_act"],
                mapping.get("speed_cmd"),
                mapping["torque"],
            )
            st.pyplot(fig_speed, clear_figure=True)

        if mapping.get("torque") and mapping.get("pos_act"):
            fig_pos = _plot_pos_vs_torque(
                subset,
                time_s,
                mapping["pos_act"],
                mapping.get("pos_cmd"),
                mapping["torque"],
            )
            st.pyplot(fig_pos, clear_figure=True)


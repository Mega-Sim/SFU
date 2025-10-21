from __future__ import annotations

import re
from dataclasses import dataclass
from io import StringIO
from typing import Dict, Iterable, List, Optional

import numpy as np
import pandas as pd

from .parser import iter_logs


_COLUMN_KEYWORDS = {
    "time_ms": {"time", "ms", "msec", "tick", "timestamp"},
    "command_position": {"cmdpos", "commandposition", "commandpos", "cmdposition"},
    "actual_position": {"realpos", "actualpos", "actpos", "feedbackpos", "presentpos"},
    "command_velocity": {"cmdvel", "cmdspeed", "commandvel", "commandvelocity"},
    "actual_velocity": {"realvel", "actualvel", "actvel", "feedbackvel", "presentvel"},
    "torque_percent": {"torque", "torquepercent", "tq", "tqpercent"},
}

_KV_PATTERNS = {
    "time_ms": re.compile(r"(?:time|ms|tick)\s*[:=]\s*(-?\d+(?:\.\d+)?)", re.I),
    "command_position": re.compile(r"cmd(?:_?|\s*)(?:pos|position)\s*[:=]\s*(-?\d+(?:\.\d+)?)", re.I),
    "actual_position": re.compile(r"(?:real|act|fb)(?:_?|\s*)(?:pos|position)\s*[:=]\s*(-?\d+(?:\.\d+)?)", re.I),
    "command_velocity": re.compile(r"cmd(?:_?|\s*)(?:vel|velocity|speed)\s*[:=]\s*(-?\d+(?:\.\d+)?)", re.I),
    "actual_velocity": re.compile(r"(?:real|act|fb)(?:_?|\s*)(?:vel|velocity|speed)\s*[:=]\s*(-?\d+(?:\.\d+)?)", re.I),
    "torque_percent": re.compile(r"(?:torque|tq)(?:\(\%\))?\s*[:=]\s*(-?\d+(?:\.\d+)?)", re.I),
}


def _clean_tokens(line: str) -> List[str]:
    tokens = re.split(r"[\s,;\t]+", line.strip())
    return [tok for tok in tokens if tok]


def _normalise_column(name: str) -> Optional[str]:
    raw = re.sub(r"[^a-z0-9%]", "", name.lower())
    for target, keywords in _COLUMN_KEYWORDS.items():
        if any(raw.startswith(key) or key in raw for key in keywords):
            return target
    return None


def _build_dataframe_from_table(text: str) -> pd.DataFrame:
    header: Optional[List[str]] = None
    rows: List[List[str]] = []
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line or line.startswith(("#", "//", ";")):
            continue
        tokens = _clean_tokens(line)
        if not tokens:
            continue
        if header is None and any(re.search(r"[A-Za-z]", tok) for tok in tokens):
            header = tokens
            continue
        if header is None:
            # No explicit header detected yet; continue searching.
            continue
        if len(tokens) < len(header):
            continue
        rows.append(tokens[: len(header)])

    if not rows or header is None:
        return pd.DataFrame()

    df = pd.read_csv(StringIO("\n".join([",".join(header)] + [",".join(row) for row in rows])), engine="python")

    rename_map: Dict[str, str] = {}
    for col in df.columns:
        mapped = _normalise_column(str(col))
        if mapped:
            rename_map[col] = mapped
    df = df.rename(columns=rename_map)
    return df


def _build_dataframe_from_key_values(text: str) -> pd.DataFrame:
    records: List[Dict[str, float]] = []
    for raw_line in text.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        values: Dict[str, float] = {}
        for key, rx in _KV_PATTERNS.items():
            m = rx.search(line)
            if m:
                try:
                    values[key] = float(m.group(1))
                except ValueError:
                    continue
        if values:
            records.append(values)
    if not records:
        return pd.DataFrame()
    return pd.DataFrame(records)


def _ensure_numeric(df: pd.DataFrame) -> pd.DataFrame:
    for col in df.columns:
        if df[col].dtype.kind in ("i", "f"):
            continue
        df[col] = pd.to_numeric(df[col], errors="coerce")
    df = df.dropna(how="all")
    return df


def _augment_timebase(df: pd.DataFrame) -> pd.DataFrame:
    if "time_ms" not in df.columns:
        df = df.reset_index().rename(columns={"index": "time_index"})
        df["time_ms"] = df["time_index"].astype(float)
    df = df.sort_values("time_ms")
    df = df.drop_duplicates(subset="time_ms")
    origin = df["time_ms"].iloc[0]
    df["time_offset_ms"] = df["time_ms"] - origin
    df["time_sec"] = df["time_ms"] / 1000.0
    df["time_offset_sec"] = df["time_offset_ms"] / 1000.0
    return df


def _detect_command_events(df: pd.DataFrame) -> List[float]:
    candidates: List[pd.Series] = []
    if "command_position" in df.columns:
        candidates.append(df["command_position"].diff().abs())
    if "command_velocity" in df.columns:
        candidates.append(df["command_velocity"].diff().abs())
    if not candidates:
        return []

    combined = pd.concat(candidates, axis=1, ignore_index=True).max(axis=1)
    finite = combined.replace([np.inf, -np.inf], np.nan).dropna()
    if finite.empty:
        return []
    threshold = max(finite.std() * 2, finite.max() * 0.15, 1e-3)
    mask = combined >= threshold
    times = df.loc[mask, "time_ms"].dropna().unique()
    return sorted(float(t) for t in times)


def parse_trace_text(text: str) -> pd.DataFrame:
    df_table = _build_dataframe_from_table(text)
    df = df_table if not df_table.empty else _build_dataframe_from_key_values(text)
    if df.empty:
        return df
    df = _ensure_numeric(df)
    numeric_cols = [col for col in df.columns if df[col].dtype.kind in ("i", "f")]
    df = df[numeric_cols]
    if df.empty:
        return df
    df = df.dropna(how="all")
    if df.empty:
        return df
    df = _augment_timebase(df)
    return df


@dataclass
class TraceDataset:
    file: str
    category: str
    axis: Optional[str]
    frame: pd.DataFrame
    error_times: List[float]
    command_times: List[float]


def collect_trace_datasets(paths: Iterable, rules, result: Dict[str, object]) -> List[TraceDataset]:
    traces: List[TraceDataset] = []
    axis_rx = re.compile(r"AXIS\[(\d)\]")
    error_times = [float(a["ts"]) for a in result.get("anchors", []) if a.get("ts") is not None]

    for fname, text in iter_logs(paths):
        category = rules.categorize(fname)
        if category not in {"트레이스_C", "트레이스_M"}:
            continue
        df = parse_trace_text(text)
        if df.empty:
            continue
        m = axis_rx.search(fname)
        axis_label: Optional[str] = None
        if m:
            axis_label = rules.axis_name(m.group(1))
        trace_errors = []
        if error_times:
            min_time = df["time_ms"].min()
            max_time = df["time_ms"].max()
            trace_errors = [t for t in error_times if min_time <= t <= max_time]
        command_times = _detect_command_events(df)
        traces.append(
            TraceDataset(
                file=fname,
                category=category,
                axis=axis_label,
                frame=df,
                error_times=trace_errors,
                command_times=command_times,
            )
        )
    return traces


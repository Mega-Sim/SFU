"""Helpers for code source ingestion and validation."""

from __future__ import annotations

import hashlib
import io
import zipfile
from dataclasses import dataclass
from typing import Iterable, List, Optional


@dataclass
class SourceBundle:
    """Container describing an uploaded or fetched source bundle."""

    filename: str
    zipbytes: bytes
    namelist: List[str]
    sha256: str
    origin: Optional[dict] = None

    @property
    def file_count(self) -> int:
        return len(self.namelist)


def _sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def _build_bundle(filename: str, raw: bytes, origin: Optional[dict] = None) -> SourceBundle:
    try:
        with zipfile.ZipFile(io.BytesIO(raw), "r") as zf:
            names = zf.namelist()
    except zipfile.BadZipFile as exc:  # pragma: no cover - simple guard
        raise ValueError(f"{filename}은(는) 올바른 ZIP 형식이 아닙니다.") from exc

    return SourceBundle(
        filename=filename,
        zipbytes=raw,
        namelist=names,
        sha256=_sha256(raw),
        origin=origin,
    )


def load_zip(uploaded_file) -> SourceBundle:
    """Normalize a Streamlit UploadedFile to a SourceBundle."""

    try:
        uploaded_file.seek(0)
    except Exception:  # pragma: no cover - Streamlit specific guard
        pass
    raw = uploaded_file.read()
    if not raw:
        raise ValueError(f"{uploaded_file.name}이(가) 비어 있습니다.")
    return _build_bundle(uploaded_file.name, raw, origin={"type": "upload"})


def basic_validate(bundle: SourceBundle, expected_keywords: Iterable[str]) -> List[str]:
    """Simple heuristic validation ensuring filenames contain expected keywords."""

    haystack = " ".join(bundle.namelist).lower()
    missing = []
    for keyword in expected_keywords:
        if keyword.lower() not in haystack:
            missing.append(keyword)
    return missing


def summarize_source(vehicle: SourceBundle | None, motion: SourceBundle | None, mode: str) -> dict:
    """Return a compact summary suitable for UI display or metadata."""

    summary = {"mode": mode}
    if vehicle:
        summary["vehicle"] = {
            "filename": vehicle.filename,
            "sha256": vehicle.sha256,
            "file_count": vehicle.file_count,
        }
        if vehicle.origin:
            summary["vehicle"].update(vehicle.origin)
    if motion:
        summary["motion"] = {
            "filename": motion.filename,
            "sha256": motion.sha256,
            "file_count": motion.file_count,
        }
        if motion.origin:
            summary["motion"].update(motion.origin)
    return summary


def make_bundle_from_bytes(name: str, data: bytes, origin: Optional[dict] = None) -> SourceBundle:
    """Expose bundle creation for non-upload flows (e.g. Git import)."""

    return _build_bundle(name, data, origin=origin)

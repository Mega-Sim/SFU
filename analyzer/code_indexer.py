from __future__ import annotations

import io
import re
import zipfile
from pathlib import Path
from typing import Dict, Iterable, Iterator, Tuple

ALLOWED_EXT = (
    ".h",
    ".hpp",
    ".c",
    ".cpp",
    ".cc",
    ".cs",
    ".txt",
    ".ini",
    ".md",
    ".json",
    ".xml",
    ".yml",
    ".yaml",
)

RX_DEFINE_ERR = re.compile(r"#\s*define\s+(ERR_[A-Z0-9_]+)\s+(\d+)")
RX_ENUM_BLOCK = re.compile(r"enum\s+\w*\s*\{([^}]+)\};", re.S)
RX_ENUM_KV = re.compile(r"(ERR_[A-Z0-9_]+)\s*=\s*(\d+)")
RX_CS_CONST = re.compile(r"\bpublic\s+const\s+int\s+(ERR_[A-Z0-9_]+)\s*=\s*(\d+)\s*;")


def _decode_bytes(data: bytes) -> str | None:
    for enc in ("utf-8", "cp949", "latin-1"):
        try:
            return data.decode(enc)
        except Exception:
            continue
    return None


def _iter_code_texts_from_zip(zip_bytes: bytes) -> Iterator[Tuple[str, str]]:
    with zipfile.ZipFile(io.BytesIO(zip_bytes), "r") as zf:
        for info in zf.infolist():
            if info.is_dir():
                continue
            if not any(info.filename.lower().endswith(ext) for ext in ALLOWED_EXT):
                continue
            try:
                raw = zf.read(info)
            except Exception:
                continue
            text = _decode_bytes(raw)
            if text is None:
                continue
            yield info.filename, text


def _first_line(text: str, needle: str) -> int:
    for idx, line in enumerate(text.splitlines(), start=1):
        if needle in line:
            return idx
    return -1


def _scan_err_maps(texts: Iterable[Tuple[str, str]]) -> Dict[str, Dict]:
    map_num_to_name: Dict[str, str] = {}
    map_name_to_num: Dict[str, str] = {}
    provenance: Dict[str, list] = {}

    for filename, text in texts:
        for name, num in RX_DEFINE_ERR.findall(text):
            map_num_to_name[num] = name
            map_name_to_num[name] = num
            provenance.setdefault(num, []).append(
                {"file": filename, "kind": "define", "line": _first_line(text, name)}
            )
        for block in RX_ENUM_BLOCK.findall(text):
            for name, num in RX_ENUM_KV.findall(block):
                map_num_to_name[num] = name
                map_name_to_num[name] = num
                provenance.setdefault(num, []).append({"file": filename, "kind": "enum-kv"})
        for name, num in RX_CS_CONST.findall(text):
            map_num_to_name[num] = name
            map_name_to_num[name] = num
            provenance.setdefault(num, []).append({"file": filename, "kind": "cs-const"})

    return {
        "map_num_to_name": map_num_to_name,
        "map_name_to_num": map_name_to_num,
        "provenance": provenance,
    }


def build_source_index(
    *, vehicle_zip_bytes: bytes | None = None, motion_zip_bytes: bytes | None = None
) -> Dict[str, Dict]:
    """Build a combined or partial source index from ZIP bundles."""

    if vehicle_zip_bytes is None and motion_zip_bytes is None:
        raise ValueError("최소 하나 이상의 코드 ZIP이 필요합니다.")

    result: Dict[str, Dict] = {}
    required_sources: list[str] = []

    if vehicle_zip_bytes is not None:
        vehicle_texts = list(_iter_code_texts_from_zip(vehicle_zip_bytes))
        vehicle_index = _scan_err_maps(vehicle_texts)
        if not vehicle_index["map_num_to_name"]:
            raise ValueError("vehicle_control.zip에서 ERR/E### 매핑을 추출하지 못했습니다.")
        result["vehicle"] = vehicle_index
        required_sources.append("vehicle")

    if motion_zip_bytes is not None:
        motion_texts = list(_iter_code_texts_from_zip(motion_zip_bytes))
        motion_index = _scan_err_maps(motion_texts)
        if not motion_index["map_num_to_name"]:
            raise ValueError("motion_control.zip에서 ERR/E### 매핑을 추출하지 못했습니다.")
        result["motion"] = motion_index
        required_sources.append("motion")

    if not required_sources:
        raise ValueError("최소 하나 이상의 코드 ZIP이 필요합니다.")

    result["meta"] = {
        "required_sources": required_sources,
        "cycle_ms": 1,
    }
    return result


def build_source_index_from_paths(vehicle_zip_path: Path, motion_zip_path: Path) -> Dict[str, Dict]:
    """Helper for offline/default indexing using file paths."""

    if not vehicle_zip_path.exists():
        raise FileNotFoundError(vehicle_zip_path)
    if not motion_zip_path.exists():
        raise FileNotFoundError(motion_zip_path)

    vehicle_bytes = vehicle_zip_path.read_bytes()
    motion_bytes = motion_zip_path.read_bytes()
    return build_source_index(vehicle_zip_bytes=vehicle_bytes, motion_zip_bytes=motion_bytes)

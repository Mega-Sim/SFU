
from __future__ import annotations
import zipfile, io, re
from pathlib import Path
from typing import Dict, List, Optional

ALLOWED_EXT = (".h",".hpp",".c",".cpp",".cc",".cs",".txt",".ini",".md",".json",".xml",".yml",".yaml")

RX_DEFINE_ERR = re.compile(r"#\s*define\s+(ERR_[A-Z0-9_]+)\s+(\d+)")
RX_ENUM_BLOCK = re.compile(r"enum\s+\w*\s*\{([^}]+)\};", re.S)
RX_ENUM_KV     = re.compile(r"(ERR_[A-Z0-9_]+)\s*=\s*(\d+)")
RX_CS_CONST    = re.compile(r"\bpublic\s+const\s+int\s+(ERR_[A-Z0-9_]+)\s*=\s*(\d+)\s*;")

def _read_text_from_zip_member(z: zipfile.ZipFile, member: str) -> Optional[str]:
    try:
        data = z.read(member)
        return data.decode("utf-8", errors="ignore")
    except Exception:
        try:
            return data.decode("cp949", errors="ignore")
        except Exception:
            return None

def _iter_code_files(path: Path):
    if path.is_dir():
        for p in sorted(path.rglob("*")):
            if p.is_file() and p.suffix.lower() in ALLOWED_EXT:
                try:
                    yield str(p), p.read_text(encoding="utf-8", errors="ignore")
                except Exception:
                    yield str(p), p.read_text(encoding="cp949", errors="ignore")
    elif path.suffix.lower() == ".zip":
        with zipfile.ZipFile(path, "r") as z:
            for info in z.infolist():
                if info.is_dir(): continue
                if not any(info.filename.lower().endswith(ext) for ext in ALLOWED_EXT):
                    continue
                txt = _read_text_from_zip_member(z, info.filename)
                if txt is not None:
                    yield f"{path.name}:{info.filename}", txt
    else:
        if path.suffix.lower() in ALLOWED_EXT:
            try:
                yield path.name, path.read_text(encoding="utf-8", errors="ignore")
            except Exception:
                yield path.name, path.read_text(encoding="cp949", errors="ignore")

def _first_line(text: str, needle: str) -> int:
    for i, ln in enumerate(text.splitlines(), 1):
        if needle in ln:
            return i
    return -1

def index_code(paths: List[Path]) -> Dict:
    out = {"map_num_to_name": {}, "map_name_to_num": {}, "provenance": {}}
    for p in paths:
        for fname, txt in _iter_code_files(p):
            for name, num in RX_DEFINE_ERR.findall(txt):
                out.setdefault("map_num_to_name", {})[num] = name
                out.setdefault("map_name_to_num", {})[name] = num
                out.setdefault("provenance", {}).setdefault(num, []).append({"file": fname, "kind": "define", "line": _first_line(txt, name)})
            for block in RX_ENUM_BLOCK.findall(txt):
                for name, num in RX_ENUM_KV.findall(block):
                    out.setdefault("map_num_to_name", {})[num] = name
                    out.setdefault("map_name_to_num", {})[name] = num
                    out.setdefault("provenance", {}).setdefault(num, []).append({"file": fname, "kind": "enum-kv"})
            for name, num in RX_CS_CONST.findall(txt):
                out.setdefault("map_num_to_name", {})[num] = name
                out.setdefault("map_name_to_num", {})[name] = num
                out.setdefault("provenance", {}).setdefault(num, []).append({"file": fname, "kind": "cs-const"})
    return out

from __future__ import annotations
import io, re, zipfile
from pathlib import Path
from typing import Iterable, Tuple

TIME_RX = re.compile(r"\[(\d{2}):(\d{2}):(\d{2})(?:\.(\d{1,3}))?\]")

def to_ms(h, m, s, ms="0"):
    return ((int(h)*60+int(m))*60+int(s))*1000 + int(str(ms or "0").ljust(3,"0"))

def iter_logs(paths) -> Iterable[Tuple[str, str]]:
    for p in paths:
        p = Path(p)
        if p.is_dir():
            for q in sorted(p.rglob("*")):
                yield from _iter_one(q)
        else:
            yield from _iter_one(p)

def _iter_one(p: Path):
    name = str(p.name)
    if p.suffix.lower() == ".zip":
        with zipfile.ZipFile(p, "r") as z:
            for info in z.infolist():
                if info.is_dir(): continue
                data = z.read(info)
                inner_name = f"{p.name}:{info.filename}"
                if info.filename.lower().endswith(".log.zip"):
                    try:
                        with zipfile.ZipFile(io.BytesIO(data), "r") as inner:
                            for info2 in inner.infolist():
                                if info2.is_dir(): continue
                                txt = inner.read(info2).decode("utf-8", errors="ignore")
                                yield (f"{inner_name}:{info2.filename}", txt)
                    except Exception:
                        pass
                else:
                    try:
                        txt = data.decode("utf-8", errors="ignore")
                    except Exception:
                        txt = data.decode("cp949", errors="ignore")
                    yield (inner_name, txt)
    else:
        try:
            txt = p.read_text(encoding="utf-8")
        except Exception:
            txt = p.read_text(encoding="cp949", errors="ignore")
        yield (name, txt)

def find_time_ms(line: str):
    m = TIME_RX.search(line)
    if not m: return None
    h, M, s, ms = m.groups()
    return to_ms(h, M, s, ms or "0")

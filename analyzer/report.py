from __future__ import annotations
from typing import Dict, Any

def ms_to_hms(ms: int|None) -> str|None:
    if ms is None: return None
    hh = ms//3600000; ms%=3600000
    mm = ms//60000; ms%=60000
    ss = ms//1000; ms%=1000
    return f"{hh:02d}:{mm:02d}:{ss:02d}.{ms:03d}"

def banner_lines(banner, error_map) -> str:
    lines=[]
    for b in banner:
        code = str(b["code"])
        name = error_map.get(code, "")
        lines.append(
            f"- E{code} {f'({name})' if name else ''}: count={b['count']} "
            f"window={ms_to_hms(b['first'])} ~ {ms_to_hms(b['last'])} | "
            f"precursor={'YES' if b['precursor_present'] else 'NO'} | "
            f"driving={'YES' if b['drive_evidence'] else 'UNSURE'}"
        )
    return "\n".join(lines)

def one_line(rec: Dict[str,Any]) -> str:
    return f"[{ms_to_hms(rec.get('ts'))}] {rec.get('file')} :: {rec.get('text')}"

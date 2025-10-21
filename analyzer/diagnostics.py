from __future__ import annotations

import zipfile
from pathlib import Path
import re
from typing import Dict, Iterable, List, Tuple

from .report import ms_to_hms


def _build_path_maps(meta: Dict) -> Tuple[Dict[str, Path], List[Path]]:
    path_map: Dict[str, Path] = {}
    dir_paths: List[Path] = []
    for raw in meta.get("paths", []):
        p = Path(raw)
        path_map[p.name] = p
        path_map[str(p)] = p
        if p.is_dir():
            dir_paths.append(p)
    return path_map, dir_paths


def _read_text_from_candidate(path: Path) -> str | None:
    try:
        return path.read_text(encoding="utf-8")
    except Exception:
        try:
            return path.read_text(encoding="cp949")
        except Exception:
            return None


def _load_source_file(file_ref: str, meta: Dict) -> str | None:
    path_map, dir_paths = _build_path_maps(meta)

    if ":" in file_ref and file_ref.split(":", 1)[0].endswith(".zip"):
        zip_name, inner = file_ref.split(":", 1)
        zp = path_map.get(zip_name)
        if zp and zp.is_file():
            try:
                with zipfile.ZipFile(zp, "r") as zf:
                    data = zf.read(inner)
            except KeyError:
                data = None
            if data is not None:
                for enc in ("utf-8", "cp949"):
                    try:
                        return data.decode(enc)
                    except Exception:
                        continue
        # fall back to directories
        for base in dir_paths:
            candidate = base / inner
            if candidate.exists():
                txt = _read_text_from_candidate(candidate)
                if txt is not None:
                    return txt
        return None

    p = Path(file_ref)
    if p.exists():
        txt = _read_text_from_candidate(p)
        if txt is not None:
            return txt

    for base in dir_paths:
        candidate = base / file_ref
        if candidate.exists():
            txt = _read_text_from_candidate(candidate)
            if txt is not None:
                return txt

    return None


def _context_from_source(file_ref: str, line_no: int, meta: Dict, window: int = 3) -> List[Dict]:
    if line_no is None or line_no <= 0:
        return []
    text = _load_source_file(file_ref, meta)
    if text is None:
        return []
    lines = text.splitlines()
    start = max(line_no - window - 1, 0)
    end = min(line_no + window, len(lines))
    ctx = []
    for idx in range(start, end):
        ctx.append({"lineno": idx + 1, "text": lines[idx]})
    return ctx


def _collect_source_context(code: str, name: str, code_index: Dict, max_blocks: int = 2) -> List[Dict]:
    provenance = code_index.get("provenance", {}).get(code, [])
    meta = code_index.get("meta", {})
    blocks = []
    for prov in provenance[:max_blocks]:
        ctx = _context_from_source(prov.get("file", ""), prov.get("line", -1), meta)
        if ctx:
            blocks.append({"file": prov.get("file", ""), "context": ctx})
    if blocks:
        return blocks

    # Fallback: try to search the code base for the error name directly.
    meta_paths = meta.get("paths", [])
    seen = 0
    for raw in meta_paths:
        p = Path(raw)
        if p.is_file() and p.suffix == ".zip":
            try:
                with zipfile.ZipFile(p, "r") as zf:
                    for info in zf.infolist():
                        if info.is_dir():
                            continue
                        try:
                            data = zf.read(info.filename)
                        except KeyError:
                            continue
                        for enc in ("utf-8", "cp949"):
                            try:
                                txt = data.decode(enc)
                                break
                            except Exception:
                                txt = None
                        if not txt or name not in txt:
                            continue
                        lines = txt.splitlines()
                        for idx, line in enumerate(lines, 1):
                            if name in line:
                                segment = lines[max(idx - 4, 0): min(idx + 3, len(lines))]
                                blocks.append({
                                    "file": f"{p.name}:{info.filename}",
                                    "context": [{"lineno": max(idx - 4, 0) + i + 1, "text": seg}
                                                 for i, seg in enumerate(segment)]
                                })
                                seen += 1
                                break
                        if seen >= max_blocks:
                            return blocks
            except zipfile.BadZipFile:
                continue
        elif p.is_dir():
            for candidate in p.rglob("*"):
                if not candidate.is_file():
                    continue
                try:
                    txt = candidate.read_text(encoding="utf-8")
                except Exception:
                    try:
                        txt = candidate.read_text(encoding="cp949")
                    except Exception:
                        continue
                if name not in txt:
                    continue
                lines = txt.splitlines()
                for idx, line in enumerate(lines, 1):
                    if name in line:
                        segment = lines[max(idx - 4, 0): min(idx + 3, len(lines))]
                        blocks.append({
                            "file": str(candidate),
                            "context": [{"lineno": max(idx - 4, 0) + i + 1, "text": seg}
                                         for i, seg in enumerate(segment)]
                        })
                        seen += 1
                        break
                if seen >= max_blocks:
                    return blocks
    return blocks


def _flatten_context(blocks: List[Dict]) -> List[str]:
    snippets: List[str] = []
    for blk in blocks:
        prefix = blk.get("file", "")
        for row in blk.get("context", []):
            snippets.append(f"{prefix}:{row['lineno']}: {row['text'].strip()}")
    return snippets


def _keyword_hints(tokens: Iterable[str]) -> Tuple[str | None, List[str]]:
    joined = " ".join(tokens).lower()
    hints: List[Tuple[List[str], str, str]] = [
        (["servo", "off"], "축 서보가 Disable 상태로 확인됩니다.", "서보 앰프 전원 및 비상정지 회로, 서보 온 신호를 확인하고 재투입합니다."),
        (["bumper"], "안전 범퍼가 눌린 상태입니다.", "차량 주변 장애물을 제거하고 범퍼 센서를 복구합니다."),
        (["amp", "fault"], "축 앰프 Fault 신호가 감지되었습니다.", "해당 축 앰프 알람을 리셋하고 전원/신호선을 점검합니다."),
        (["limit"], "소프트/하드 리미트 조건을 위반했습니다.", "리미트 스위치/리미트 영역을 점검하고 기계적 간섭을 제거합니다."),
        (["sensor", "on"], "센서 상태 이상으로 판단됩니다.", "센서 배선 및 동작 상태를 확인합니다."),
        (["handpio", "go"], "Hand PIO GO 신호가 오프 상태입니다.", "설비 PIO 신호 수신상태와 인터록을 확인합니다."),
        (["commun", "timeout"], "통신 지연 또는 단절이 발생했습니다.", "네트워크 케이블, 스위치 및 통신 상태를 점검합니다."),
        (["ethernet"], "이더넷 연결 불량으로 보입니다.", "LAN 케이블 및 커넥터 접촉 상태를 점검합니다."),
        (["battery"], "배터리 또는 전원 저하가 감지되었습니다.", "전원 모듈 및 배터리 상태를 점검합니다."),
    ]
    for keys, cause, action in hints:
        if all(key in joined for key in keys):
            return cause, [action]
    return None, []


def _axis_description(name: str, axis_map: Dict[str, str]) -> str | None:
    import re

    m = re.search(r"AXIS(\d)", name)
    if not m:
        return None
    axis = m.group(1)
    if axis in axis_map:
        return f"관련 축: {axis_map[axis]}(AXIS{axis})"
    return f"관련 축: AXIS{axis}"


def _summarize_precursors(entries: List[Dict]) -> List[str]:
    summary = []
    for p in entries[:3]:
        delta = p.get("dt_ms")
        desc = p.get("text", "").strip()
        file = p.get("file", "")
        summary.append(f"{file} (Δt={delta}ms): {desc}")
    return summary


def _summarize_drive(entries: List[Dict]) -> List[str]:
    out = []
    for d in entries[:3]:
        out.append(f"{d.get('file','')} @ {ms_to_hms(d.get('ts'))}: {d.get('text','').strip()}")
    return out


def _extract_vehicle(text: str) -> str | None:
    patterns = [
        re.compile(r"\b(?:OHT|AGV|VEH(?:ICLE)?|CAR)[-_ ]?(\d{1,4})\b", re.I),
        re.compile(r"\bV(?:EHICLE)?[:= ]?(\d{1,4})\b", re.I),
        re.compile(r"\bCAR_ID[:= ]?(\d{1,4})\b", re.I),
    ]
    for rx in patterns:
        m = rx.search(text)
        if m:
            return f"{m.group(1)}번"
    return None


def _extract_node(text: str) -> str | None:
    patterns = [
        re.compile(r"\bNODE[:= ]?([A-Z0-9_-]{2,})\b", re.I),
        re.compile(r"\bSTATION[:= ]?([A-Z0-9_-]{2,})\b", re.I),
        re.compile(r"\bPORT[:= ]?([A-Z0-9_-]{2,})\b", re.I),
        re.compile(r"\bN(\d{2,})\b"),
    ]
    for rx in patterns:
        m = rx.search(text)
        if m:
            return m.group(1)
    return None


def _extract_activity(text: str) -> str | None:
    keywords = [
        (re.compile(r"\b(LOAD|UNLOAD|LIFT|DROP|HOIST)\b", re.I), "이적재"),
        (re.compile(r"\b(TRANSFER|PASS)\b", re.I), "이동"),
        (re.compile(r"\b(DRIVE|RUN|MOVE|TRAVEL)\b", re.I), "주행"),
        (re.compile(r"\b(DOCK|ALIGN)\b", re.I), "도킹"),
    ]
    for rx, label in keywords:
        if rx.search(text):
            return label
    return None


def _extract_sensor(text: str) -> str | None:
    rx_list = [
        re.compile(r"\b([A-Z0-9_]+SENSOR)\b"),
        re.compile(r"\b([A-Z0-9_]+_SIG)\b"),
        re.compile(r"([가-힣A-Za-z0-9_]+센서)"),
    ]
    for rx in rx_list:
        m = rx.search(text)
        if m:
            return m.group(1)
    return None


def _describe_source(snippet: str) -> Tuple[str, str, str]:
    try:
        ref, lineno, code_line = snippet.rsplit(":", 2)
        return ref.strip(), lineno.strip(), code_line.strip()
    except ValueError:
        return snippet, "", ""


def _build_scenario(
    anchors: List[Dict],
    precursors: List[Dict],
    drive: List[Dict],
    source_snippets: List[str],
) -> str:
    anchor_texts = [a.get("text", "") for a in anchors]
    drive_texts = [d.get("text", "") for d in drive]
    combined_text = " ".join(anchor_texts + drive_texts)

    vehicle = _extract_vehicle(combined_text)
    node = _extract_node(combined_text)
    activity = _extract_activity(combined_text)
    sensor = _extract_sensor(combined_text)

    subject = "해당 차량"
    if vehicle:
        subject = f"{vehicle} 차량"

    node_phrase = ""
    if node:
        node_phrase = node if node.endswith("노드") else f"{node} 노드"

    activity_phrase = ""
    if activity:
        activity_phrase = f"{activity} 작업 중 "

    focus_phrase = "관련 신호"
    if sensor:
        focus_phrase = sensor if "센서" in sensor else f"{sensor} 센서"

    location_part = f"{node_phrase}에서 " if node_phrase else ""
    subject_part = f"{subject}은"

    def _clean_log_text(text: str) -> str:
        trimmed = re.sub(r"\s+", " ", text.strip())
        if len(trimmed) > 120:
            return trimmed[:117] + "..."
        return trimmed

    timeline: List[str] = []
    timeline.append(
        f"{subject_part} {location_part}{activity_phrase}{focus_phrase} 이상 징후를 감지했습니다.".strip()
    )

    if precursors:
        for precursor in sorted(precursors, key=lambda p: (p.get("dt_ms") is None, -(p.get("dt_ms") or 0))):
            delta = precursor.get("dt_ms")
            delta_phrase = f"사건 {delta}ms 전에 " if delta is not None else "사건 직전에 "
            log_time = ms_to_hms(precursor.get("ts"))
            time_phrase = f"({log_time}) " if log_time else ""
            log_ref = precursor.get("file", "")
            log_text = _clean_log_text(precursor.get("text", ""))
            timeline.append(
                f"{delta_phrase}{time_phrase}{log_ref} 로그에 \"{log_text}\"가 기록되어 초기 전조로 확인되었습니다."
            )

    if anchors:
        first_anchor = anchors[0]
        anchor_ts = ms_to_hms(first_anchor.get("ts"))
        anchor_time = anchor_ts if anchor_ts else "해당 시점"
        anchor_ref = first_anchor.get("file", "")
        anchor_text = _clean_log_text(first_anchor.get("text", ""))
        timeline.append(
            f"{anchor_time}에 {anchor_ref} 로그에서 \"{anchor_text}\" 메시지가 출력되며 오류가 발생했습니다."
        )

    if drive:
        drive_hint = drive[0]
        drive_ts = ms_to_hms(drive_hint.get("ts"))
        drive_time = drive_ts if drive_ts else "동일 구간"
        drive_ref = drive_hint.get("file", "")
        drive_text = _clean_log_text(drive_hint.get("text", ""))
        timeline.append(
            f"이후 {drive_time} 주행 로그({drive_ref})에서는 \"{drive_text}\" 흐름이 이어져 현장 상황을 뒷받침합니다."
        )

    if source_snippets:
        ref, lineno, code_line = _describe_source(source_snippets[0])
        if code_line:
            timeline.append(
                f"제어 로직 {ref}:{lineno}에서는 \"{code_line}\" 조건으로 동일 상황을 방지하도록 설계되어 있어 추가 검토가 필요합니다."
            )
        else:
            timeline.append(
                f"제어 로직 {ref}에 따른 기대 동작과 실제 로그를 비교하여 원인을 재확인하세요."
            )

    timeline.append("위 흐름을 기반으로 센서/배선 및 주변 인터락 상태를 우선 점검하세요.")

    return "\n".join(part for part in timeline if part)


def generate_diagnostic_report(result: Dict, rules) -> List[Dict]:
    diagnostics: List[Dict] = []
    code_index = getattr(rules, "code_index", {}) or {}
    axis_map = rules.rules.get("axis_map", {})

    for banner in result.get("banner", []):
        code = str(banner.get("code"))
        name = rules.error_map.get(code, "")
        anchors = [a for a in result.get("anchors", []) if str(a.get("code")) == code]
        precursors = [p for p in result.get("precursors", []) if str(p.get("code")) == code]
        drive = [d for d in result.get("drive_samples", []) if str(d.get("code")) == code]

        source_blocks = _collect_source_context(code, name, code_index)
        source_snippets = _flatten_context(source_blocks)
        tokens: List[str] = [name]
        for sn in source_snippets:
            tokens.append(sn)
        cause, actions = _keyword_hints(tokens)
        axis_info = _axis_description(name, axis_map)

        summary_parts = [
            f"총 {banner.get('count', 0)}회 발생",
            f"구간 {ms_to_hms(banner.get('first'))} ~ {ms_to_hms(banner.get('last'))}",
        ]
        if axis_info:
            summary_parts.append(axis_info)

        if not cause:
            cause = "소스 코드 스니펫을 확인하여 상세 원인을 판단하세요."

        if not actions:
            actions = ["관련 하드웨어 상태와 인터록을 점검하고 이상 시 재기동 절차를 수행합니다."]

        scenario = _build_scenario(anchors, precursors, drive, source_snippets)

        diagnostics.append({
            "code": code,
            "name": name,
            "summary": ", ".join(summary_parts),
            "root_cause": cause,
            "actions": actions,
            "scenario": scenario,
            "precursors": _summarize_precursors(precursors),
            "drive": _summarize_drive(drive),
            "log_samples": [a.get("text", "") for a in anchors[:3]],
            "code_snippets": source_snippets,
        })

    return diagnostics


from __future__ import annotations
import io
import json
import zipfile
from pathlib import Path
from typing import Any, Dict, Optional

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data"
DEFAULT_SYSTEM_DIR = ROOT / "default_system"
DATA.mkdir(parents=True, exist_ok=True)

RULE_FILE = DATA / "ruleset.json"
FEEDBACK_FILE = DATA / "feedback.json"
MODEL_FILE = DATA / "model.joblib"
SOURCE_INDEX_FILE = DATA / "source_index.json"

_DEFAULT_INDEX_CACHE: Optional[Dict[str, Any]] = None

REQUIRED_SOURCES = ("vehicle", "motion")

def load_json(path: Path, default: Any) -> Any:
    if path.exists():
        try:
            return json.loads(path.read_text(encoding="utf-8"))
        except Exception:
            return default
    return default

def save_json(path: Path, obj: Any) -> None:
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2), encoding="utf-8")

def load_rules() -> Dict[str, Any]:
    return load_json(RULE_FILE, default={
        "version": "v1.0",
        "time_window_sec": {"precursor_before": 3, "precursor_after": 1, "anchor_merge": 2},
        "categories": {
            "마스터로그": r"^\[master\]_.*\.log$",
            "트레이스_C": r"AMC_AXIS\[\d\]_C_TRACE_.*\.log(\.zip)?$",
            "트레이스_M": r"AMC_AXIS\[\d\]_M_TRACE_.*\.log(\.zip)?$",
            "AMC_Recv": r"AMC_Recv.*\.log(\.zip)?$",
            "MCC": r"MCC.*\.log(\.zip)?$",
            "User": r"User.*\.log(\.zip)?$",
            "AMC_Send_Periodic": r"AMC_Send_Periodic.*\.log(\.zip)?$",
            "AMC_Send": r"AMC_Send(?!_Periodic).*\.log(\.zip)?$",
            "Assistant": r"Assistant.*\.log(\.zip)?$",
            "AutoRecovery": r"AutoRecovery.*\.log(\.zip)?$",
            "BCR": r"(?<!RawData)BCR.*\.log(\.zip)?$",
            "BCR_RawData": r"BCR.*RawData.*\.log(\.zip)?$",
            "CarrierID": r"CarrierID.*\.log(\.zip)?$",
            "CID-LOG": r"CID-?LOG.*\.log(\.zip)?$",
            "CmdManager": r"CmdManager.*\.log(\.zip)?$",
            "CPUandMemInfo": r"CPUandMemInfo.*\.log(\.zip)?$",
            "DETECT": r"(?<!OHTDETECTWarnning)DETECT.*\.log(\.zip)?$",
            "DiagManager": r"DiagManager.*\.log(\.zip)?$",
            "DrivingCtrl": r"DrivingCtrl.*\.log(\.zip)?$",
            "EQPIOError": r"EQPIOError.*\.log(\.zip)?$",
            "Execute": r"(?<!ExecuteJobThread)Execute.*\.log(\.zip)?$",
            "ExecuteJobThread": r"ExecuteJobThread.*\.log(\.zip)?$",
            "FM": r"FM.*\.log(\.zip)?$",
            "HIDRawData": r"HIDRawData.*\.log(\.zip)?$",
            "IOTComm": r"IOTComm.*\.log(\.zip)?$",
            "IOTHUB": r"IOTHUB.*\.log(\.zip)?$",
            "ManualControl": r"ManualControl.*\.log(\.zip)?$",
            "Monitor": r"Monitor.*\.log(\.zip)?$",
            "MonitoringDetail": r"MonitoringDetail.*\.log(\.zip)?$",
            "OHTDETECTWarnning": r"OHTDETECTWarnning.*\.log(\.zip)?$",
            "Passpermit": r"Passpermit.*\.log(\.zip)?$",
            "PathSearch": r"PathSearch.*\.log(\.zip)?$",
            "QRTest": r"QRTest.*\.log(\.zip)?$",
            "Shutter": r"Shutter.*\.log(\.zip)?$",
            "SOS_Rcv_RawData": r"SOS[_-]?Rcv[_-]?RawData.*\.log(\.zip)?$",
            "ThreadCycle": r"ThreadCycle.*\.log(\.zip)?$",
            "TaskControl": r"TaskControl.*\.log(\.zip)?$",
            "UBGPatternCom": r"UBGPatternCom.*\.log(\.zip)?$|UBGPatternComp.*\.log(\.zip)?$",
            "UDPCommunication": r"UDPCommunication.*\.log(\.zip)?$",
            "WirelessNet": r"WirelessNet.*\.log(\.zip)?$"
        },
        "error_patterns": {
            "anchor": [r"\[E\s*(\d{3})\]", r"Error\s*[:=]\s*(\d{3})"],
            "confirm_map": {"960": "ERR_AXIS2_SERVO_OFFED", "464": "ERR_BUMPER_PRESS"}
        },
        "precursor_patterns": [
            r"previously sent frames are received/processed\s*\(frame loss\)!",
            r"Ethernet cable not connected",
            r"\blink\s+(?:is\s+)?down\b",
            r"carrier\s+lost",
            r"PHY\s+reset",
            r"\bCRC\s+error\b",
            r"(?:rx|tx)\s+(?:drop|error|lost)",
            r"\bdisconnect(?:ed)?\b",
            r"\breconnect\b",
            r"\btimeout\b"
        ],
        "confusion_whitelist": [
            r"\bNe\d{3,}\b", r"\bCha\d{3,}\b", r"\bNode\d+\b", r"\bChannel\d+\b"
        ],
        "drive_keywords": [r"\bDRIVE\b", r"\bRUN\b", r"\bVEL\b", r"\bSPEED\b", r"\bMOVE\b", r"\bACC\b", r"\bDCC\b"],
        "axis_map": {"0": "Driving-Rear", "1": "Driving-Front", "2": "Hoist", "3": "Slide"},
        "terminology": {
            "Mark": "마크 또는 마킹은 vehicle이 근처 노드로 정위치를 잡기 위하여 이동하는 동작입니다.",
            "small add": "마킹하거나 차량이 극히 짧은 거리를 조정할 필요가 있을 때 수행되는 미세 이동 동작을 의미합니다."
        }
    })

def save_rules(rules: Dict[str, Any]) -> None:
    save_json(RULE_FILE, rules)

def load_feedback() -> Dict[str, Any]:
    return load_json(FEEDBACK_FILE, default={"items": []})

def save_feedback(fb: Dict[str, Any]) -> None:
    save_json(FEEDBACK_FILE, fb)

def _load_default_system_index() -> Dict[str, Any]:
    global _DEFAULT_INDEX_CACHE
    if _DEFAULT_INDEX_CACHE is not None:
        return _DEFAULT_INDEX_CACHE

    vehicle_bytes = _read_default_system_zip_bytes("vehicle_control")
    motion_bytes = _read_default_system_zip_bytes("motion_control")
    if not vehicle_bytes or not motion_bytes:
        _DEFAULT_INDEX_CACHE = {}
        return _DEFAULT_INDEX_CACHE

    from analyzer.code_indexer import build_source_index  # Local import to avoid circular dependency.

    idx = build_source_index(vehicle_zip_bytes=vehicle_bytes, motion_zip_bytes=motion_bytes)
    idx.setdefault("meta", {})["source"] = "default_system"
    _DEFAULT_INDEX_CACHE = idx
    return _DEFAULT_INDEX_CACHE


def _read_default_system_zip_bytes(name: str) -> bytes | None:
    zip_path = DEFAULT_SYSTEM_DIR / f"{name}.zip"
    if zip_path.exists():
        try:
            return zip_path.read_bytes()
        except Exception:
            return None

    dir_path = DEFAULT_SYSTEM_DIR / name
    if not dir_path.exists() or not dir_path.is_dir():
        return None

    buffer = io.BytesIO()
    try:
        with zipfile.ZipFile(buffer, "w") as zf:
            for path in dir_path.rglob("*"):
                if path.is_file():
                    arcname = str(path.relative_to(dir_path))
                    zf.write(path, arcname=arcname)
    except Exception:
        return None
    return buffer.getvalue()


def _available_sections(data: Dict[str, Any]) -> list[str]:
    sections: list[str] = []
    for key in REQUIRED_SOURCES:
        section = data.get(key)
        if not isinstance(section, dict):
            continue
        map_num_to_name = section.get("map_num_to_name")
        if isinstance(map_num_to_name, dict) and map_num_to_name:
            sections.append(key)
    return sections


def _is_valid_source_index(
    data: Dict[str, Any], required: tuple[str, ...] | None = None
) -> bool:
    if not isinstance(data, dict):
        return False

    if required is None:
        meta_required = data.get("meta", {}).get("required_sources")
        if isinstance(meta_required, (list, tuple)) and meta_required:
            required = tuple(str(r) for r in meta_required)
        else:
            required = REQUIRED_SOURCES

    if required:
        for key in required:
            section = data.get(key)
            if not isinstance(section, dict):
                return False
            map_num_to_name = section.get("map_num_to_name")
            if not isinstance(map_num_to_name, dict) or not map_num_to_name:
                return False
        return True

    return bool(_available_sections(data))


def load_source_index() -> Dict[str, Any]:
    data = load_json(SOURCE_INDEX_FILE, default={})
    if _is_valid_source_index(data):
        return data

    default_idx = _load_default_system_index()
    if _is_valid_source_index(default_idx):
        return default_idx
    return {}


def save_source_index(obj: Dict[str, Any]) -> None:
    valid_sections = _available_sections(obj)
    if not valid_sections:
        raise ValueError("source_index.json must include at least one non-empty section")

    meta = obj.setdefault("meta", {})
    required = meta.get("required_sources")
    if not isinstance(required, (list, tuple)) or not required:
        meta["required_sources"] = valid_sections
    else:
        meta["required_sources"] = [str(r) for r in required]

    meta["cycle_ms"] = 1

    if not _is_valid_source_index(obj, tuple(meta["required_sources"])):
        raise ValueError("source_index.json must include the configured required sections")

    save_json(SOURCE_INDEX_FILE, obj)


def required_sources_present(required: tuple[str, ...] | None = None) -> bool:
    return _is_valid_source_index(load_source_index(), required)

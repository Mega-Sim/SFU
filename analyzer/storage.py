from __future__ import annotations
import json
from pathlib import Path
from typing import Any, Dict, List, Optional

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data"
DEFAULT_SYSTEM_DIR = ROOT / "default_system"
DATA.mkdir(parents=True, exist_ok=True)

RULE_FILE = DATA / "ruleset.json"
FEEDBACK_FILE = DATA / "feedback.json"
MODEL_FILE = DATA / "model.joblib"
SOURCE_INDEX_FILE = DATA / "source_index.json"

_DEFAULT_INDEX_CACHE: Optional[Dict[str, Any]] = None

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
        "axis_map": {"0": "Driving-Rear", "1": "Driving-Front", "2": "Hoist", "3": "Slide"}
    })

def save_rules(rules: Dict[str, Any]) -> None:
    save_json(RULE_FILE, rules)

def load_feedback() -> Dict[str, Any]:
    return load_json(FEEDBACK_FILE, default={"items": []})

def save_feedback(fb: Dict[str, Any]) -> None:
    save_json(FEEDBACK_FILE, fb)

def _default_system_code_paths() -> List[Path]:
    paths: List[Path] = []
    if DEFAULT_SYSTEM_DIR.exists():
        # Prefer the packaged ZIPs for faster indexing. Fallback to directories.
        for name in ("vehicle_control.zip", "motion_control.zip"):
            p = DEFAULT_SYSTEM_DIR / name
            if p.exists():
                paths.append(p)
        for name in ("vehicle_control", "motion_control"):
            p = DEFAULT_SYSTEM_DIR / name
            if p.exists():
                paths.append(p)
    return paths


def _load_default_system_index() -> Dict[str, Any]:
    global _DEFAULT_INDEX_CACHE
    if _DEFAULT_INDEX_CACHE is not None:
        return _DEFAULT_INDEX_CACHE

    code_paths = _default_system_code_paths()
    if not code_paths:
        _DEFAULT_INDEX_CACHE = {"map_num_to_name": {}, "map_name_to_num": {}, "provenance": {}}
        return _DEFAULT_INDEX_CACHE

    from analyzer.code_indexer import index_code  # Local import to avoid circular dependency.

    idx = index_code(code_paths)
    idx.setdefault("meta", {})["source"] = "default_system"
    idx.setdefault("meta", {})["paths"] = [str(p) for p in code_paths]
    _DEFAULT_INDEX_CACHE = idx
    return _DEFAULT_INDEX_CACHE


def load_source_index() -> Dict[str, Any]:
    data = load_json(
        SOURCE_INDEX_FILE,
        default={"map_num_to_name": {}, "map_name_to_num": {}, "provenance": {}},
    )
    if data.get("map_num_to_name"):
        return data

    default_idx = _load_default_system_index()
    if default_idx.get("map_num_to_name"):
        return default_idx
    return data

def save_source_index(obj: Dict[str, Any]) -> None:
    save_json(SOURCE_INDEX_FILE, obj)

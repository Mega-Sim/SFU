"""Application configuration loader."""

from __future__ import annotations

from pathlib import Path
from typing import Any, Dict

import yaml

_DEFAULT_CONFIG = {
    "require_both_code_zips": True,
    "allow_git_sources": False,
    "git": {
        "default_vehicle_repo": "",
        "default_motion_repo": "",
        "default_ref": "main",
    },
}


def load_config(path: str | Path = "config/app.yaml") -> Dict[str, Any]:
    """Load YAML configuration with sensible defaults."""

    cfg_path = Path(path)
    if not cfg_path.exists():
        return dict(_DEFAULT_CONFIG)

    try:
        text = cfg_path.read_text(encoding="utf-8")
    except OSError:
        return dict(_DEFAULT_CONFIG)

    try:
        loaded = yaml.safe_load(text) or {}
    except yaml.YAMLError:
        return dict(_DEFAULT_CONFIG)

    if not isinstance(loaded, dict):
        return dict(_DEFAULT_CONFIG)

    merged: Dict[str, Any] = dict(_DEFAULT_CONFIG)
    for key, value in loaded.items():
        if isinstance(value, dict) and isinstance(merged.get(key), dict):
            nested = dict(merged[key])
            nested.update(value)
            merged[key] = nested
        else:
            merged[key] = value
    return merged

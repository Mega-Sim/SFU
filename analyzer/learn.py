from __future__ import annotations
from typing import List
from .storage import load_feedback, save_feedback, load_rules, save_rules

def add_feedback(case_name: str, comments: str, new_precursors: List[str]|None=None, new_confusions: List[str]|None=None):
    fb = load_feedback()
    fb["items"].append({
        "case": case_name,
        "comments": comments,
        "new_precursors": new_precursors or [],
        "new_confusions": new_confusions or []
    })
    save_feedback(fb)

    rules = load_rules()
    changed = False
    if new_precursors:
        rules["precursor_patterns"] = list(set(rules["precursor_patterns"] + new_precursors)); changed=True
    if new_confusions:
        rules["confusion_whitelist"] = list(set(rules["confusion_whitelist"] + new_confusions)); changed=True
    if changed:
        save_rules(rules)
    return rules

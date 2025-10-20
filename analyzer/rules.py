from __future__ import annotations
import re
from typing import Dict, List, Tuple

class RuleSet:
    def __init__(self, rules: Dict, code_index: Dict|None=None):
        self.rules = rules
        self.code_index = code_index or {"map_num_to_name": {}}
        self._cat_rx = {k: re.compile(v, re.I) for k,v in rules["categories"].items()}
        self._anchor_rx = [re.compile(p, re.I) for p in rules["error_patterns"]["anchor"]]
        self._precursor_rx = [re.compile(p, re.I) for p in rules["precursor_patterns"]]
        self._conf_whitelist = [re.compile(p, re.I) for p in rules["confusion_whitelist"]]
        self._drive_rx = [re.compile(p, re.I) for p in rules["drive_keywords"]]

    def categorize(self, filename: str) -> str:
        for cat, rx in self._cat_rx.items():
            if rx.search(filename):
                return cat
        return "기타"

    def match_anchors(self, line: str) -> List[Tuple[str,int]]:
        out=[]
        for rx in self._anchor_rx:
            m = rx.search(line)
            if m:
                code = m.group(1)
                for w in self._conf_whitelist:
                    if w.search(line):
                        return []
                out.append((line, int(code)))
        return out

    def is_precursor(self, line: str) -> bool:
        return any(rx.search(line) for rx in self._precursor_rx)

    def is_drive_hint(self, line: str) -> bool:
        return any(rx.search(line) for rx in self._drive_rx)

    def axis_name(self, idx: str) -> str:
        return self.rules["axis_map"].get(idx, idx)

    @property
    def windows(self):
        return self.rules["time_window_sec"]

    @property
    def error_map(self):
        em = self.rules["error_patterns"].get("confirm_map", {}).copy()
        for num, name in self.code_index.get("map_num_to_name", {}).items():
            em[str(num)] = name
        return em

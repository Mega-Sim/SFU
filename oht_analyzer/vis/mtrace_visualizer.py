from __future__ import annotations
import io, re, math, tarfile, zipfile
from dataclasses import dataclass
from pathlib import Path
from typing import List, Optional, Dict, Tuple, Iterable
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# --- detection ---
MTRACE_NAME = re.compile(r"(?i)\bM[-_]?TRACE\b")
ENC = ("utf-8-sig","utf-8","cp949","euc-kr","latin-1")
# AMC headerless index profile (현장 포맷 대응)
AMC_IDX = {"time":0, "speed_cmd":3, "speed_act":4, "torque":5, "pos_cmd":6, "pos_act":9}

TOKENS = {
  "time":["time","timestamp","t","ms","time_ms","tick","sample","시간","타임"],
  "torque":["torque","trq","iq","iqref","iqfb","current","load","토크","전류"],
  "speed_act":["actualspeed","actspeed","speed","velocity","actvel","feedbackspeed","fbvel","velfb","실제속도","피드백속도"],
  "speed_cmd":["commandspeed","cmdspeed","targetspeed","refspeed","setpointspeed","velcmd","vref","명령속도","지령속도"],
  "pos_act":["actualposition","actposition","position","actpos","feedbackposition","fbpos","posfb","실제위치","피드백위치"],
  "pos_cmd":["commandposition","cmdposition","targetposition","refposition","setpointposition","cmdpos","poscmd","명령위치","지령위치"],
}
def _norm(s:str)->str: return re.sub(r"[^a-z0-9가-힣]+","", s.lower())
def _find_col(cols: List[str], keys: List[str])->Optional[str]:
    n={c:_norm(c) for c in cols}; k=[_norm(x) for x in keys]
    for c,v in n.items():
        for t in k:
            if v==t or v.startswith(t): return c
    for c,v in n.items():
        for t in k:
            if t in v: return c
    return None

def _read_csvlike(b: bytes) -> pd.DataFrame:
    last=None
    for enc in ENC:
        try:
            bio=io.BytesIO(b)
            df=pd.read_csv(bio, sep=None, engine="python", encoding=enc)
            if df.shape[1]==1:
                bio.seek(0); df=pd.read_csv(bio, delim_whitespace=True, engine="python", encoding=enc)
            return df
        except Exception as e:
            last=e
    raise RuntimeError(last)

def _build_time(df: pd.DataFrame, time_col: Optional[str]) -> pd.Series:
    if time_col and time_col in df:
        s=df[time_col]; s2=pd.to_numeric(s, errors="coerce")
        if s2.notna().mean()>0.9:
            med=s2.dropna().median()
            return (s2/1000.0) if 1000<med<1e9 else s2
        try:
            ts=pd.to_datetime(s); return (ts-ts.iloc[0]).dt.total_seconds()
        except Exception: pass
    return pd.Series(np.arange(len(df))*0.001, name="time_s")

def _downsample(df: pd.DataFrame, max_points:int)->pd.DataFrame:
    n=len(df)
    if n<=max_points: return df
    step=math.ceil(n/max_points); return df.iloc[::step,:].reset_index(drop=True)

def _plot_dual(time_s, lefts, labels, right, ylabel_left, title, outp: Path):
    fig=plt.figure(figsize=(12,5))
    ax1=plt.gca()
    for s,l in zip(lefts,labels): ax1.plot(time_s, s, label=l)
    ax1.set_xlabel("Time (s)"); ax1.set_ylabel(ylabel_left); ax1.legend(loc="upper left")
    ax2=ax1.twinx(); ax2.plot(time_s, right, label="Torque", alpha=0.85); ax2.set_ylabel("Torque")
    plt.title(title); fig.tight_layout(); fig.savefig(outp,dpi=150); plt.close(fig)

def _iter_streams(bundle: Path):
    if bundle.is_dir():
        for fp in bundle.rglob("*"):
            if fp.is_file(): yield (str(fp), fp.read_bytes()); 
        return
    low=bundle.name.lower()
    if low.endswith(".zip"):
        with zipfile.ZipFile(bundle,"r") as zp:
            for n in zp.namelist(): yield (n, zp.read(n))
        return
    if low.endswith((".tar",".tar.gz",".tgz",".tar.bz2",".tbz2",".tar.xz",".txz")):
        with tarfile.open(bundle,"r:*") as tp:
            for m in tp.getmembers():
                if not m.isfile(): continue
                f=tp.extractfile(m); 
                if f: yield (m.name, f.read())
        return
    if bundle.is_file(): yield (bundle.name, bundle.read_bytes())

@dataclass
class VisualizeResult:
    source: str
    speed_png: Optional[Path]
    pos_png: Optional[Path]
    notes: str

def _by_header_or_profile(name:str, b:bytes)->Tuple[pd.DataFrame, Dict[str,Optional[str]], str]:
    """Return (df, mapping, mode) where mode in {'header','amc_idx'}."""
    # try header first
    df=_read_csvlike(b)
    cols=list(df.columns)
    mapping={
        "time":_find_col(cols,TOKENS["time"]),
        "torque":_find_col(cols,TOKENS["torque"]),
        "speed_act":_find_col(cols,TOKENS["speed_act"]),
        "speed_cmd":_find_col(cols,TOKENS["speed_cmd"]),
        "pos_act":_find_col(cols,TOKENS["pos_act"]),
        "pos_cmd":_find_col(cols,TOKENS["pos_cmd"]),
    }
    if any(mapping.values()):
        return df, mapping, "header"
    # AMC headerless (all numeric, name has AMC_AXIS)
    text=io.BytesIO(b).getvalue().decode("latin-1","ignore")
    lines=[ln for ln in text.splitlines() if ln.strip()]
    nums=[]
    import re as _re
    for ln in lines[:200]:
        try: nums.append([float(x) for x in _re.split(r"[\t,\s]+", ln.strip()) if x!=""])
        except: pass
    if nums and len(nums[0])>max(AMC_IDX.values()) and re.search(r"(?i)AMC[_-]?AXIS", Path(name).name):
        rows=[]
        for ln in lines:
            try: rows.append([float(x) for x in _re.split(r"[\t,\s]+", ln.strip()) if x!=""])
            except: pass
        arr=np.array(rows, dtype=float)
        df=pd.DataFrame(arr); df.columns=[f"{i}" for i in range(df.shape[1])]
        mapping={k:f"{AMC_IDX[k]}" for k in AMC_IDX}
        return df, mapping, "amc_idx"
    return df, mapping, "header"

def visualize_from_bundle(bundle_path, out_dir, smooth_window=1, max_points=120_000):
    bundle=Path(bundle_path); out=Path(out_dir); out.mkdir(parents=True, exist_ok=True)
    results=[]; found=False
    for name, data in _iter_streams(bundle):
        base=Path(name).name
        if not MTRACE_NAME.search(base): continue
        found=True
        try:
            df, mapping, mode=_by_header_or_profile(base, data)
            keep=[c for c in [mapping.get("time"),mapping.get("torque"),mapping.get("speed_act"),mapping.get("speed_cmd"),mapping.get("pos_act"),mapping.get("pos_cmd")] if c]
            if not keep:
                results.append(VisualizeResult(name,None,None,"No recognizable columns")); continue
            # time
            if mode=="header":
                t=_build_time(df, mapping.get("time"))
            else:
                t=pd.to_numeric(df[mapping["time"]], errors="coerce")
                dt=float(t.diff().median()) if len(t)>1 else 1.0
                scale=1000.0 if 1.0<=dt<=20.0 else 1.0
                t=(t - t.iloc[0]) / scale
            # numeric + downsample
            for c in keep: df[c]=pd.to_numeric(df[c], errors="coerce")
            tmp=pd.DataFrame({"__t__":t}); [tmp.__setitem__(c, df[c]) for c in keep]
            tmp=_downsample(tmp, max_points); t=tmp["__t__"]; df=tmp.drop(columns=["__t__"])
            safe=re.sub(r"[^a-zA-Z0-9._-]+","_", base)
            sp=out/(safe+"_speed_vs_torque.png"); ps=out/(safe+"_position_vs_torque.png")
            # speed vs torque
            if mapping.get("torque") and mapping.get("speed_act"):
                left=[df[mapping["speed_act"]]]; labels=["Actual Speed"]
                if mapping.get("speed_cmd"): left.append(df[mapping["speed_cmd"]]); labels.append("Command Speed")
                _plot_dual(t, left, labels, df[mapping["torque"]], "Speed", "Speed vs Torque (left: speed, right: torque)", sp)
            else: sp=None
            # position vs torque
            if mapping.get("torque") and mapping.get("pos_act"):
                left=[df[mapping["pos_act"]]]; labels=["Actual Position"]
                if mapping.get("pos_cmd"): left.append(df[mapping["pos_cmd"]]); labels.append("Command Position")
                _plot_dual(t, left, labels, df[mapping["torque"]], "Position", "Position vs Torque (left: position, right: torque)", ps)
            else: ps=None
            results.append(VisualizeResult(name, sp, ps, f"mode={mode}"))
        except Exception as e:
            results.append(VisualizeResult(name, None, None, f"Error: {e!r}"))
    if not found:
        results.append(VisualizeResult(str(bundle), None, None, "No M-Trace files found"))
    return results

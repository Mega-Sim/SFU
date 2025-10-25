from __future__ import annotations

import re

import streamlit as st
from pathlib import Path
from typing import List, Optional

import altair as alt
import pandas as pd

from analyzer.storage import (
    load_rules,
    save_rules,
    load_source_index,
    save_source_index,
    required_sources_present,
)
from analyzer.rules import RuleSet
from analyzer.engine import analyze
from analyzer.report import banner_lines, one_line, ms_to_hms
from analyzer.diagnostics import generate_diagnostic_report
from analyzer.learn import add_feedback
from analyzer.code_indexer import build_source_index
from analyzer.trace import collect_trace_datasets
from analyzer.mtrace import render_mtrace_section
# ────────────────────────────────────────────────────────────────
# 아래 코드는 새 브랜치(codex)에서 추가한 부분
from analyzer.viz import configure_altair
from core.config import load_config
from core.ingest import load_zip, basic_validate, summarize_source, SourceBundle
from core.git_loader import fetch_from_git

configure_altair()
# ────────────────────────────────────────────────────────────────

cfg = load_config()

st.set_page_config(page_title="OHT 로그 분석기 (로그 + 코드 참조)", layout="wide")

st.markdown("## ✅ OHT 로그 분석기 — 증거-우선 / 보수적 결론 / 피드백 학습 / **코드 ZIP 자동참조**")
st.caption("축: 0=Driving-Rear, 1=Driving-Front, 2=Hoist, 3=Slide | 1ms 통신 | amulation/crc15_ccitt 제외")

current_idx = load_source_index()
vehicle_count = len(current_idx.get("vehicle", {}).get("map_num_to_name", {}))
motion_count = len(current_idx.get("motion", {}).get("map_num_to_name", {}))
idx_source = current_idx.get("meta", {}).get("source")

if vehicle_count and motion_count:
    if idx_source == "default_system":
        st.caption(
            f"현재 기본 시스템(motion_control + vehicle_control) 코드 매핑: vehicle {vehicle_count}건 + motion {motion_count}건을 사용 중입니다."
        )
    else:
        st.caption(
            f"현재 저장된 코드 매핑 적용 중 — vehicle {vehicle_count}건, motion {motion_count}건."
        )
else:
    st.caption("현재 코드 매핑이 비어 있습니다.")

with st.sidebar:
    st.markdown("### 설정 / 룰셋")
    rules_obj = load_rules()
    st.text(f"RuleSet: {rules_obj.get('version','v?')}")
    st.write("전조 윈도우(초):", rules_obj["time_window_sec"])
    if st.button("룰셋 초기화(기본값)"):
        save_rules(load_rules())
        st.success("룰셋 기본값으로 리셋")

    glossary = rules_obj.get("terminology") or rules_obj.get("glossary")
    if glossary:
        with st.expander("주요 용어 정리"):
            for term, desc in glossary.items():
                st.markdown(f"**{term}**")
                st.caption(desc)

SOURCE_MODE_BOTH = "양쪽 ZIP(권장)"
SOURCE_MODE_VEHICLE = "차량만 ZIP(빠른 모드)"
SOURCE_MODE_MOTION = "모션만 ZIP(빠른 모드)"
SOURCE_MODE_GIT = "Git Import(선택)"

VALIDATION_KEYWORDS = ["err_"]

both_required = cfg.get("require_both_code_zips", True)
allow_git_sources = cfg.get("allow_git_sources", False)
git_defaults = cfg.get("git", {}) or {}

mode_options = [SOURCE_MODE_BOTH]
if not both_required:
    mode_options.extend([SOURCE_MODE_VEHICLE, SOURCE_MODE_MOTION])
if allow_git_sources:
    mode_options.append(SOURCE_MODE_GIT)

st.markdown("### 0) 코드 소스 — vehicle_control + motion_control (기본: 둘 다 필요)")
st.caption(
    "두 프로그램은 1ms 주기로 상호 참조되므로 **양쪽을 함께 업로드해야 정확한 상관 분석이 가능합니다.** "
    "빠른 점검이 필요하면 ‘단독 모드’를 선택할 수 있습니다."
)

source_mode = st.radio(
    "입력 소스",
    mode_options,
    index=0,
    help="기본값은 ‘양쪽 ZIP(권장)’입니다. 단독 모드는 일부 규칙이 비활성화됩니다.",
)

if both_required and len(mode_options) > 1:
    st.info("관리자 정책으로 현재 실행은 ‘양쪽 ZIP’만 허용됩니다.", icon="🔒")
    source_mode = SOURCE_MODE_BOTH

st.session_state["source_mode"] = source_mode

vehicle_bundle: SourceBundle | None = None
motion_bundle: SourceBundle | None = None

vehicle_repo_input = git_defaults.get("default_vehicle_repo", "")
motion_repo_input = git_defaults.get("default_motion_repo", "")
vehicle_ref_input = git_defaults.get("default_ref", "main")
motion_ref_input = git_defaults.get("default_ref", "main")

if source_mode == SOURCE_MODE_VEHICLE:
    st.warning(
        "빠른 모드: 차량측 단독 규칙만 실행됩니다(교차 상관/페어링 규칙 비활성).",
        icon="⚠️",
    )
elif source_mode == SOURCE_MODE_MOTION:
    st.warning(
        "빠른 모드: 모션측 단독 규칙만 실행됩니다(교차 상관/페어링 규칙 비활성).",
        icon="⚠️",
    )

if source_mode == SOURCE_MODE_BOTH:
    zip_col_vehicle, zip_col_motion = st.columns(2)
    with zip_col_vehicle:
        vehicle_file = st.file_uploader(
            "vehicle_control.zip (필수)",
            type=["zip"],
            accept_multiple_files=False,
            key="vehicle_zip",
        )
        if vehicle_file:
            try:
                vehicle_bundle = load_zip(vehicle_file)
                if basic_validate(vehicle_bundle, VALIDATION_KEYWORDS):
                    st.warning("ERR_ 관련 정의가 포함된 파일을 찾지 못했습니다. 경로를 확인하세요.", icon="⚠️")
                st.caption(
                    f"파일 {vehicle_bundle.file_count}개 · SHA256 {vehicle_bundle.sha256[:12]}…"
                )
            except ValueError as exc:
                st.error(str(exc))
    with zip_col_motion:
        motion_file = st.file_uploader(
            "motion_control.zip (필수)",
            type=["zip"],
            accept_multiple_files=False,
            key="motion_zip",
        )
        if motion_file:
            try:
                motion_bundle = load_zip(motion_file)
                if basic_validate(motion_bundle, VALIDATION_KEYWORDS):
                    st.warning("ERR_ 관련 정의가 포함된 파일을 찾지 못했습니다. 경로를 확인하세요.", icon="⚠️")
                st.caption(
                    f"파일 {motion_bundle.file_count}개 · SHA256 {motion_bundle.sha256[:12]}…"
                )
            except ValueError as exc:
                st.error(str(exc))
elif source_mode == SOURCE_MODE_VEHICLE:
    vehicle_file = st.file_uploader(
        "vehicle_control.zip", type=["zip"], accept_multiple_files=False, key="vehicle_zip_single"
    )
    if vehicle_file:
        try:
            vehicle_bundle = load_zip(vehicle_file)
            if basic_validate(vehicle_bundle, VALIDATION_KEYWORDS):
                st.warning("ERR_ 관련 정의가 포함된 파일을 찾지 못했습니다. 경로를 확인하세요.", icon="⚠️")
            st.caption(f"파일 {vehicle_bundle.file_count}개 · SHA256 {vehicle_bundle.sha256[:12]}…")
        except ValueError as exc:
            st.error(str(exc))
elif source_mode == SOURCE_MODE_MOTION:
    motion_file = st.file_uploader(
        "motion_control.zip", type=["zip"], accept_multiple_files=False, key="motion_zip_single"
    )
    if motion_file:
        try:
            motion_bundle = load_zip(motion_file)
            if basic_validate(motion_bundle, VALIDATION_KEYWORDS):
                st.warning("ERR_ 관련 정의가 포함된 파일을 찾지 못했습니다. 경로를 확인하세요.", icon="⚠️")
            st.caption(f"파일 {motion_bundle.file_count}개 · SHA256 {motion_bundle.sha256[:12]}…")
        except ValueError as exc:
            st.error(str(exc))
elif source_mode == SOURCE_MODE_GIT:
    if not allow_git_sources:
        st.info("관리자 정책상 Git Import가 비활성화되어 있습니다.")
    git_vehicle_col, git_motion_col = st.columns(2)
    with git_vehicle_col:
        vehicle_repo_input = st.text_input(
            "Vehicle repo URL",
            value=git_defaults.get("default_vehicle_repo", ""),
            key="vehicle_git_repo",
        )
        vehicle_ref_input = st.text_input(
            "Vehicle ref (branch/tag/SHA)",
            value=git_defaults.get("default_ref", "main"),
            key="vehicle_git_ref",
        )
    with git_motion_col:
        motion_repo_input = st.text_input(
            "Motion repo URL",
            value=git_defaults.get("default_motion_repo", ""),
            key="motion_git_repo",
        )
        motion_ref_input = st.text_input(
            "Motion ref (branch/tag/SHA)",
            value=git_defaults.get("default_ref", "main"),
            key="motion_git_ref",
        )
    st.caption("입력한 저장소는 인덱싱 버튼을 누를 때 Git에서 가져옵니다.")

if vehicle_bundle or motion_bundle:
    st.caption("현재 선택한 코드 소스 요약")
    st.json(summarize_source(vehicle_bundle, motion_bundle, source_mode))

col_idx1, col_idx2 = st.columns([1, 3])
with col_idx1:
    if st.button("코드 인덱싱 ▶ (선택한 모드 적용)"):
        bundle_vehicle = vehicle_bundle
        bundle_motion = motion_bundle
        try:
            if source_mode == SOURCE_MODE_GIT:
                if not allow_git_sources:
                    st.error("Git Import가 비활성화되어 있습니다.")
                    raise RuntimeError("Git import disabled")
                with st.spinner("Git 저장소에서 코드 가져오는 중..."):
                    bundle_vehicle, bundle_motion = fetch_from_git(
                        vehicle_repo_input,
                        vehicle_ref_input,
                        motion_repo_input,
                        motion_ref_input,
                    )

            if source_mode == SOURCE_MODE_BOTH and (bundle_vehicle is None or bundle_motion is None):
                st.error("vehicle_control.zip과 motion_control.zip을 모두 업로드하세요.")
            elif source_mode == SOURCE_MODE_VEHICLE and bundle_vehicle is None:
                st.error("vehicle_control.zip을 업로드하거나 Git 저장소를 지정하세요.")
            elif source_mode == SOURCE_MODE_MOTION and bundle_motion is None:
                st.error("motion_control.zip을 업로드하거나 Git 저장소를 지정하세요.")
            else:
                with st.spinner("코드 인덱싱 중..."):
                    idx = build_source_index(
                        vehicle_zip_bytes=bundle_vehicle.zipbytes if bundle_vehicle else None,
                        motion_zip_bytes=bundle_motion.zipbytes if bundle_motion else None,
                    )
                    summary = summarize_source(bundle_vehicle, bundle_motion, source_mode)
                    meta = idx.setdefault("meta", {})
                    meta["source_summary"] = summary
                    meta["source_mode"] = source_mode
                    meta["source_policy"] = {
                        "require_both_code_zips": both_required,
                        "allow_git_sources": allow_git_sources,
                    }
                    save_source_index(idx)

                counts = []
                if bundle_vehicle and "vehicle" in idx:
                    counts.append(f"vehicle {len(idx['vehicle']['map_num_to_name'])}건")
                if bundle_motion and "motion" in idx:
                    counts.append(f"motion {len(idx['motion']['map_num_to_name'])}건")
                detail = " + ".join(counts)
                st.success("인덱싱 완료! " + detail if detail else "인덱싱 완료!")
                st.json(summary)
        except RuntimeError as exc:
            st.error(str(exc))
        except Exception as exc:
            st.exception(exc)
with col_idx2:
    if st.button("현재 코드 매핑 요약 보기"):
        idx = load_source_index()
        vehicle_preview = dict(list(idx.get("vehicle", {}).get("map_num_to_name", {}).items())[:20])
        motion_preview = dict(list(idx.get("motion", {}).get("map_num_to_name", {}).items())[:20])
        st.json(
            {
                "meta": idx.get("meta", {}),
                "vehicle": {"map_num_to_name": vehicle_preview},
                "motion": {"map_num_to_name": motion_preview},
            }
        )

required_tuple = ("vehicle", "motion") if both_required else None
if not required_sources_present(required_tuple):
    if both_required:
        st.warning("vehicle_control.zip과 motion_control.zip을 모두 인덱싱해야 로그 분석을 진행할 수 있습니다.")
    else:
        st.warning("최소 한 개 이상의 코드 ZIP을 인덱싱해야 로그 분석을 진행할 수 있습니다.")
    st.stop()

st.markdown("### 1) 로그데이터 업로드")
uploads = st.file_uploader("ZIP 또는 LOG 파일 여러 개 선택", type=["zip","log","txt"], accept_multiple_files=True)
case_name = st.text_input("케이스 이름(리포트 표기용)", value="CASE_001")

uploaded_paths: List[Path] = []
if uploads:
    tmpdir = Path("./_work"); tmpdir.mkdir(exist_ok=True)
    for f in uploads:
        p = tmpdir / f.name
        p.write_bytes(f.getbuffer())
        uploaded_paths.append(p)
    st.success(f"{len(uploaded_paths)}개 로그 파일 저장 완료")


def parse_error_code_input(raw: str) -> List[str]:
    if not raw:
        return []
    tokens = [tok.strip() for tok in re.split(r"[,\s]+", raw) if tok.strip()]
    codes: List[str] = []
    for token in tokens:
        if token.upper().startswith("E"):
            token = token[1:]
        token = token.strip()
        if token:
            codes.append(token)
    return codes


error_code_raw = st.text_input(
    "분석할 에러 코드 (선택 입력, 쉼표/공백 구분)",
    value="",
    placeholder="예: 101 또는 101, 205",
)
selected_error_codes = parse_error_code_input(error_code_raw)
target_code_set = set(selected_error_codes) if selected_error_codes else None
if selected_error_codes:
    st.caption(
        "입력된 에러 코드만 우선 분석합니다: "
        + ", ".join(f"E{code}" for code in selected_error_codes)
    )

st.markdown("### 2) 로그 분석")
colA, colB = st.columns([1,3])
with colA:
    if st.button("로그 분석 시작 ▶"):
        st.session_state["analyze_now"] = True
with colB:
    st.info("분석은 **증거-우선**으로 진행합니다. 근거 부족 시 **미확정**으로 보류합니다.")

if st.session_state.get("analyze_now") and uploaded_paths:
    with st.spinner("분석 중..."):
        rs = RuleSet(load_rules(), code_index=load_source_index())
        result = analyze(
            uploaded_paths,
            rs,
            target_codes=target_code_set,
            source_mode=st.session_state.get("source_mode"),
        )
    st.success("분석 완료!")

    st.markdown("#### ✔ 검증 배너(요약)")
    st.code(banner_lines(result["banner"], rs.error_map), language="markdown")

    diagnostics = generate_diagnostic_report(result, rs)
    if diagnostics:
        st.markdown("#### 🧠 자동 진단 요약")
        for diag in diagnostics:
            title = f"E{diag['code']}"
            if diag.get("name"):
                title += f" ({diag['name']})"
            st.markdown(f"**{title}**")
            if diag.get("scenario"):
                st.write(diag["scenario"])
            st.write(diag["summary"])
            st.write(f"추정 원인: {diag['root_cause']}")
            st.write("권장 조치:")
            for act in diag["actions"]:
                st.markdown(f"- {act}")
            if diag["precursors"]:
                st.caption("전조 이벤트")
                for p in diag["precursors"]:
                    st.code(p, language="text")
            else:
                st.caption("전조 이벤트: 발견되지 않음")
            if diag["drive"]:
                st.caption("주행 증거")
                for d in diag["drive"]:
                    st.code(d, language="text")
            if diag["code_snippets"] or diag["log_samples"]:
                with st.expander("근거 보기"):
                    if diag["log_samples"]:
                        st.write("로그 앵커 샘플")
                        for sample in diag["log_samples"]:
                            st.code(sample, language="text")
                    if diag["code_snippets"]:
                        st.write("소스 코드 근거")
                        for snippet in diag["code_snippets"]:
                            st.code(snippet, language="text")

    st.markdown("#### 🔎 코드별 타임라인 & 전조")
    for b in result["banner"]:
        code = str(b["code"]); name = rs.error_map.get(code, "")
        st.markdown(f"**E{code} {f'({name})' if name else ''}** — {ms_to_hms(b['first'])} ~ {ms_to_hms(b['last'])}, count={b['count']}")
        precs = [p for p in result["precursors"] if str(p["code"])==code]
        if precs:
            st.write(f"전조 이벤트 {len(precs)}건 (앵커 최초 대비 Δt ms):")
            for p in precs[:10]:
                st.code(one_line(p), language="text")
        else:
            st.write("전조 이벤트: 없음")
        drives = [d for d in result["drive_samples"] if str(d["code"])==code]
        if drives:
            with st.expander("주행 힌트 원문 보기"):
                for d in drives[:10]:
                    st.code(one_line(d), language="text")
        else:
            st.caption("주행 힌트: 미확정(증거 부족)")

    trace_datasets = collect_trace_datasets(uploaded_paths, rs, result)
    if trace_datasets:
        st.markdown("#### 📈 트레이스 로그 상세 분석")
        st.caption("실제/명령 궤적과 토크를 비교하고 주요 이벤트 시점을 표시합니다.")
        for trace in trace_datasets:
            df = trace.frame
            header = f"{trace.file}"
            if trace.axis:
                header += f" · 축: {trace.axis}"
            st.markdown(f"**{header}**")

            origin = df["time_ms"].iloc[0]
            error_df = pd.DataFrame({
                "time_offset_sec": [(t - origin) / 1000.0 for t in trace.error_times],
                "label": ["에러 발생"] * len(trace.error_times),
            }) if trace.error_times else pd.DataFrame(columns=["time_offset_sec", "label"])
            command_df = pd.DataFrame({
                "time_offset_sec": [(t - origin) / 1000.0 for t in trace.command_times],
                "label": ["명령 시점"] * len(trace.command_times),
            }) if trace.command_times else pd.DataFrame(columns=["time_offset_sec", "label"])

            def _marker_layer(data: pd.DataFrame, color: str, dash: Optional[List[int]] = None):
                if data.empty:
                    return None
                mark = alt.Chart(data).mark_rule(color=color, strokeDash=dash)
                return mark.encode(
                    x=alt.X("time_offset_sec:Q", title="시간 (s)"),
                    tooltip=[alt.Tooltip("label:N", title="이벤트"), alt.Tooltip("time_offset_sec:Q", title="Δt(s)")],
                )

            layers = []

            if {"actual_position", "command_position"}.intersection(df.columns):
                pos_cols = {}
                if "actual_position" in df.columns:
                    pos_cols["actual_position"] = "실제 위치"
                if "command_position" in df.columns:
                    pos_cols["command_position"] = "명령 위치"
                pos_df = df[["time_offset_sec", *pos_cols.keys()]].rename(columns=pos_cols)
                pos_melt = pos_df.melt("time_offset_sec", var_name="항목", value_name="값")
                pos_chart = alt.Chart(pos_melt).mark_line().encode(
                    x=alt.X("time_offset_sec:Q", title="시간 (s)"),
                    y=alt.Y("값:Q", title="위치"),
                    color=alt.Color("항목:N", title=""),
                    tooltip=["항목:N", alt.Tooltip("time_offset_sec:Q", title="Δt(s)"), alt.Tooltip("값:Q", title="위치")],
                )
                layers.append((pos_chart, "위치"))

            if {"actual_velocity", "command_velocity"}.intersection(df.columns):
                vel_cols = {}
                if "actual_velocity" in df.columns:
                    vel_cols["actual_velocity"] = "실제 속도"
                if "command_velocity" in df.columns:
                    vel_cols["command_velocity"] = "명령 속도"
                vel_df = df[["time_offset_sec", *vel_cols.keys()]].rename(columns=vel_cols)
                vel_melt = vel_df.melt("time_offset_sec", var_name="항목", value_name="값")
                vel_chart = alt.Chart(vel_melt).mark_line().encode(
                    x=alt.X("time_offset_sec:Q", title="시간 (s)"),
                    y=alt.Y("값:Q", title="속도"),
                    color=alt.Color("항목:N", title=""),
                    tooltip=["항목:N", alt.Tooltip("time_offset_sec:Q", title="Δt(s)"), alt.Tooltip("값:Q", title="속도")],
                )
                layers.append((vel_chart, "속도"))

            if "torque_percent" in df.columns:
                tq_chart = alt.Chart(df).mark_line(color="#9467bd").encode(
                    x=alt.X("time_offset_sec:Q", title="시간 (s)"),
                    y=alt.Y("torque_percent:Q", title="토크(%)"),
                    tooltip=[alt.Tooltip("time_offset_sec:Q", title="Δt(s)"), alt.Tooltip("torque_percent:Q", title="토크(%)")],
                )
                layers.append((tq_chart, "토크"))

            marker_layers = [
                layer
                for layer in (
                    _marker_layer(error_df, "#d62728"),
                    _marker_layer(command_df, "#1f77b4", dash=[4, 4]),
                )
                if layer is not None
            ]

            for chart, title in layers:
                combined = alt.layer(chart, *marker_layers) if marker_layers else chart
                st.altair_chart(combined.properties(height=240), use_container_width=True)
                st.caption(f"· {title} 추세")

    st.markdown("---")
    st.markdown("### 섹션별 리포트 (작업자용)")
    ordered = [
        "마스터로그","트레이스_C","트레이스_M","AMC_Recv","MCC","User","AMC_Send_Periodic","AMC_Send",
        "Assistant","AutoRecovery","BCR","BCR_RawData","CarrierID","CID-LOG","CmdManager","CPUandMemInfo",
        "DETECT","DiagManager","DrivingCtrl","EQPIOError","Execute","ExecuteJobThread","FM","HIDRawData",
        "IOTComm","IOTHUB","ManualControl","Monitor","MonitoringDetail","OHTDETECTWarnning","Passpermit",
        "PathSearch","QRTest","Shutter","SOS_Rcv_RawData","ThreadCycle","TaskControl","UBGPatternCom",
        "UDPCommunication","WirelessNet"
    ]
    sec = result["section"]
    for cat in ordered:
        if cat in sec:
            s = sec[cat]
            st.markdown(f"**- {cat} 분석결과**")
            st.write("파일:", ', '.join(sorted(set(s['files']))))
            st.write("시간:", ms_to_hms(s["first"]), "~", ms_to_hms(s["last"]))
            if s["samples"]:
                with st.expander("근거 원문 샘플"):
                    for rec in s["samples"][:8]:
                        st.code(one_line(rec), language="text")
            st.markdown('---')

    # --- M-Trace: speed/position vs torque charts ---
    render_mtrace_section()

st.markdown("### 3) 피드백 / 재학습")
with st.form("feedback_form"):
    st.write("**추가 전조 패턴**이나 **혼동어(에러 아님)**를 입력하면 즉시 룰셋에 반영됩니다. (정규식 가능)")
    fb_comment = st.text_area("피드백 메모", "")
    new_precursors = st.text_area("추가 전조 패턴 (줄바꿈 구분)", "")
    new_confusions = st.text_area("추가 혼동어(에러 아님) 패턴 (줄바꿈 구분)", "")
    submitted = st.form_submit_button("피드백 저장 & 룰 업데이트")
    if submitted:
        pc = [s.strip() for s in new_precursors.splitlines() if s.strip()]
        cf = [s.strip() for s in new_confusions.splitlines() if s.strip()]
        rules_after = add_feedback(case_name, fb_comment, pc, cf)
        st.success("피드백 저장 & 룰 업데이트 완료")
        st.json({"added_precursors": pc, "added_confusions": cf, "rules_version": rules_after.get("version","v1.0")})

if st.button("피드백 반영하여 재분석 ▶"):
    if uploads:
        with st.spinner("재분석 중..."):
            rs = RuleSet(load_rules(), code_index=load_source_index())
            result = analyze(
                [Path("./_work")],
                rs,
                target_codes=target_code_set,
                source_mode=st.session_state.get("source_mode"),
            )
        st.success("재분석 완료")
        st.code(banner_lines(result["banner"], rs.error_map), language="markdown")
    else:
        st.warning("먼저 로그 파일을 업로드하세요.")

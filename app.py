import streamlit as st
from pathlib import Path
from typing import List
from analyzer.storage import load_rules, save_rules, load_source_index, save_source_index
from analyzer.rules import RuleSet
from analyzer.engine import analyze
from analyzer.report import banner_lines, one_line, ms_to_hms
from analyzer.diagnostics import generate_diagnostic_report
from analyzer.learn import add_feedback
from analyzer.code_indexer import index_code

st.set_page_config(page_title="OHT 로그 분석기 (로그 + 코드 참조)", layout="wide")

st.markdown("## ✅ OHT 로그 분석기 — 증거-우선 / 보수적 결론 / 피드백 학습 / **코드 ZIP 자동참조**")
st.caption("축: 0=Driving-Rear, 1=Driving-Front, 2=Hoist, 3=Slide | 1ms 통신 | amulation/crc15_ccitt 제외")

current_idx = load_source_index()
current_idx_count = len(current_idx.get("map_num_to_name", {}))
idx_source = current_idx.get("meta", {}).get("source")
if idx_source == "default_system" and current_idx_count:
    st.caption(
        f"현재 기본 시스템(motion_control + vehicle_control) 코드 매핑 {current_idx_count}건을 사용 중입니다."
    )
elif current_idx_count:
    st.caption(f"현재 저장된 코드 매핑 {current_idx_count}건이 적용됩니다.")
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

st.markdown("### 0) 코드 ZIP 업로드 — vehicle_control.zip, motion_control.zip (선택)")
code_files = st.file_uploader("코드 ZIP 업로드(여러 개 가능)", type=["zip"], accept_multiple_files=True)
code_paths: List[Path] = []
if code_files:
    srcdir = Path("./_code"); srcdir.mkdir(exist_ok=True)
    for f in code_files:
        p = srcdir / f.name
        p.write_bytes(f.getbuffer())
        code_paths.append(p)
    st.success(f"코드 ZIP {len(code_paths)}개 저장 완료")

col_idx1, col_idx2 = st.columns([1,3])
with col_idx1:
    if st.button("코드 인덱싱 ▶ (에러코드 매핑 자동 생성)"):
        if not code_paths:
            st.warning("먼저 코드 ZIP을 업로드하세요.")
        else:
            with st.spinner("코드 인덱싱 중..."):
                idx = index_code(code_paths)
                save_source_index(idx)
            st.success(f"인덱싱 완료! 매핑 {len(idx['map_num_to_name'])}건")
with col_idx2:
    if st.button("현재 코드 매핑 요약 보기"):
        idx = load_source_index()
        preview = dict(list(idx.get('map_num_to_name',{}).items())[:20])
        st.json({"meta": idx.get("meta", {}), "map_num_to_name": preview})

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
        result = analyze(uploaded_paths, rs)
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
            result = analyze([Path("./_work")], rs)
        st.success("재분석 완료")
        st.code(banner_lines(result["banner"], rs.error_map), language="markdown")
    else:
        st.warning("먼저 로그 파일을 업로드하세요.")

# OHT Log Analyzer (증거-우선 / 코드 ZIP 자동참조)

## 기능 개요
- **로그 분석:** `E###` 앵커 검출, 전조 이벤트(frame loss, Ethernet cable not connected 등), 시간창 상관, 보수적 결론
- **코드 자동참조:** `vehicle_control.zip`, `motion_control.zip` 업로드 후 **코드 인덱싱 ▶** 하면
  소스의 `ERR_*` ↔ 숫자 매핑을 자동 생성하여 리포트에 `E960 (ERR_...)`처럼 **정확 명칭으로 표기**
- **피드백 학습:** 전조/혼동어 패턴을 UI에서 추가 → 룰셋 즉시 업데이트 → **재분석**

## 실행
```bash
pip install -r requirements.txt
streamlit run app.py
```

## 사용 순서
1) (선택) **코드 ZIP 업로드**: `vehicle_control.zip`, `motion_control.zip`
2) **코드 인덱싱 ▶**: 에러코드 매핑 자동 생성
3) **로그 업로드**: ZIP/LOG 여러 개 가능(내부의 `.log.zip`도 자동 파싱)
4) **로그 분석 시작 ▶**: 배너/섹션별 결과 + 근거 원문 확인
5) **피드백 저장 & 룰 업데이트** → **재분석 ▶**

## 폴더 구조
```text
app.py
analyzer/
  ├─ parser.py         # ZIP/LOG 재귀 파싱, 타임스탬프 추출
  ├─ rules.py          # 카테고리/앵커/전조/혼동어/구동 힌트 정규식
  ├─ engine.py         # 앵커 윈도우링, 전조 Δt, 섹션 요약(증거-우선)
  ├─ report.py         # 배너/원문라인 포매터
  ├─ storage.py        # 룰/피드백/코드인덱스 저장/로드
  ├─ learn.py          # 피드백 -> 룰 업데이트
  └─ code_indexer.py   # 코드 ZIP 인덱싱(에러코드 매핑)
data/
  ├─ ruleset.json      # 초기 룰셋(전조/혼동어/카테고리/축매핑 등)
  ├─ feedback.json     # 피드백 누적
  └─ source_index.json # 코드 인덱싱 결과(에러코드 매핑)
```

## 주의
- **보수적 결론**: 근거 부족시 ‘미확정’로 남깁니다.
- **축 매핑**: 0=Driving-Rear, 1=Driving-Front, 2=Hoist, 3=Slide
- amulation/crc15_ccitt 제외

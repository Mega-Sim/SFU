# OHT Log Analyzer (증거-우선 / 코드 ZIP **필수** 참조)

## 기능 개요
- **로그 분석:** `E###` 앵커 검출, 전조 이벤트(frame loss, Ethernet cable not connected 등), 시간창 상관, 보수적 결론
- **코드 필수 참조:** `vehicle_control.zip`과 `motion_control.zip`을 **동시에 업로드** 후 **코드 인덱싱 ▶** 하면
  소스의 `ERR_*` ↔ 숫자 매핑을 자동 생성하여 리포트에 `E960 (ERR_...)`처럼 **정확 명칭으로 표기**
- **시스템 가정:** `vehicle_control` ↔ `motion_control` 간 주기 통신은 **1 ms**로 고정되어 있으며, 분석 타임라인/상관 로직의 기본 단위로 사용됩니다.
- **피드백 학습:** 전조/혼동어 패턴을 UI에서 추가 → 룰셋 즉시 업데이트 → **재분석**

## 실행
```bash
pip install -r requirements.txt
streamlit run app.py
```

## 사용 순서
1) (필수) **코드 ZIP 업로드**: `vehicle_control.zip` **AND** `motion_control.zip`
2) **코드 인덱싱 ▶**: 통합 에러코드 매핑 생성(키: `vehicle`, `motion`)
3) **로그 업로드**: ZIP/LOG 여러 개 가능(내부의 `.log.zip`도 자동 파싱)
4) **로그 분석 시작 ▶**: 배너/섹션별 결과 + 근거 원문 확인
5) **피드백 저장 & 룰 업데이트** → **재분석 ▶**

### 왜 두 파일이 필수인가?
- 실제 시스템에서 두 프로그램은 1 ms 주기로 상호 의존적으로 동작하며, 한 쪽만 보면 에러명/축라벨/상태전이 해석이 불완전해질 수 있습니다.
- 에러코드/축매핑/상태머신이 소스별로 상이할 수 있어 **양쪽 소스를 동시에 참조**해야 오탐·미탐을 줄일 수 있습니다.

### 호환성
- 과거 캐시에 `source_index.json`이 단일 소스 기준으로 존재하더라도, 새 버전은 `vehicle` + `motion` **둘 다**가 존재해야 분석이 시작됩니다.

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

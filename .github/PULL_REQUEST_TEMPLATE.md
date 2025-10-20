## 개요
- 무엇을, 왜 변경했는지 요약

## 주요 변경사항
- [ ] UI: 업로드/분석/피드백 루틴
- [ ] 코드 ZIP 인덱싱(에러코드 매핑)
- [ ] 섹션별 리포트(작업자 포맷)
- [ ] 룰셋/피드백 저장

## 테스트
- [ ] 로컬에서 `streamlit run app.py` 실행
- [ ] vehicle_control.zip, motion_control.zip 업로드 → 코드 인덱싱 ▶
- [ ] 샘플 로그 업로드 → 로그 분석 ▶
- [ ] 피드백 입력 → 재분석 ▶

## 영향도
- 모듈: analyzer/*, app.py, data/*
- 외부 의존: streamlit, pandas, scikit-learn 등 (requirements.txt 참고)

## 롤백 계획
- 문제가 있으면 `git revert <commit>` 또는 PR revert 사용

## 체크리스트
- [ ] README 반영
- [ ] .gitignore 적절
- [ ] 민감정보/대용량 로그 커밋 금지

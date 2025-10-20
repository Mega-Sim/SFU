#!/usr/bin/env bash
set -euo pipefail

# 사용법:
#   ./init_push_and_pr.sh https://github.com/Mega-Sim/SFU.git
# 사전조건: gh CLI 로그인 완료(gh auth login)

REMOTE_URL="${1:-https://github.com/Mega-Sim/SFU.git}"
BRANCH="feat/oht-log-analyzer-v1"

# 1) Git 초기화 및 첫 커밋
git init -b main
git add .
git commit -m "feat: initial OHT log analyzer v1 (logs + code indexing + feedback UI)" || true

# 2) 브랜치 생성
git checkout -B "$BRANCH"

# 3) 원격 설정 & 푸시
git remote remove origin 2>/dev/null || true
git remote add origin "$REMOTE_URL"
git push -u origin "$BRANCH"

# 4) PR 생성 (gh CLI)
if command -v gh >/dev/null 2>&1; then
  gh pr create     --base main     --head "$BRANCH"     --title "feat: initial OHT log analyzer v1"     --body-file .github/PULL_REQUEST_TEMPLATE.md || true
  echo "✔ PR 생성 시도 완료 (gh)."
else
  echo "⚠ gh가 없어 웹으로 PR을 여세요:"
  echo "   https://github.com/Mega-Sim/SFU/compare/main...Mega-Sim:${BRANCH}?expand=1"
fi

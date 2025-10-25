"""Git import helpers for source bundles."""

from __future__ import annotations

import io
import subprocess
import tempfile
import zipfile
from pathlib import Path
from typing import Tuple

from .ingest import SourceBundle, make_bundle_from_bytes


def _zip_directory(path: Path) -> bytes:
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, "w", zipfile.ZIP_DEFLATED) as zf:
        for item in path.rglob("*"):
            if not item.is_file():
                continue
            if any(part.startswith(".git") for part in item.parts):
                continue
            zf.write(item, arcname=str(item.relative_to(path)))
    return buffer.getvalue()


def _clone_repo(repo: str, ref: str | None) -> tuple[Path, str, tempfile.TemporaryDirectory]:
    tmpdir = tempfile.TemporaryDirectory()
    repo_path = Path(tmpdir.name) / "repo"
    clone_cmd = ["git", "clone", repo, str(repo_path)]
    clone_proc = subprocess.run(clone_cmd, capture_output=True, text=True)
    if clone_proc.returncode != 0:
        tmpdir.cleanup()
        raise RuntimeError(f"Git clone 실패: {clone_proc.stderr.strip() or clone_proc.stdout.strip()}")

    resolved_ref = ""
    if ref:
        checkout_proc = subprocess.run(
            ["git", "-C", str(repo_path), "checkout", ref], capture_output=True, text=True
        )
        if checkout_proc.returncode != 0:
            tmpdir.cleanup()
            raise RuntimeError(f"git checkout 실패: {checkout_proc.stderr.strip() or checkout_proc.stdout.strip()}")
        rev_proc = subprocess.run(
            ["git", "-C", str(repo_path), "rev-parse", "HEAD"], capture_output=True, text=True
        )
        if rev_proc.returncode == 0:
            resolved_ref = rev_proc.stdout.strip()
    else:
        rev_proc = subprocess.run(
            ["git", "-C", str(repo_path), "rev-parse", "HEAD"], capture_output=True, text=True
        )
        if rev_proc.returncode == 0:
            resolved_ref = rev_proc.stdout.strip()

    return repo_path, resolved_ref or (ref or "") , tmpdir


def _bundle_repo(kind: str, repo: str, ref: str | None) -> SourceBundle | None:
    if not repo:
        return None

    repo_path, resolved_ref, tmpdir = _clone_repo(repo, ref)
    try:
        zip_bytes = _zip_directory(repo_path)
        origin = {"type": "git", "repo": repo, "ref": resolved_ref or (ref or "")}
        bundle = make_bundle_from_bytes(f"{kind}_from_git.zip", zip_bytes, origin=origin)
    finally:
        tmpdir.cleanup()
    return bundle


def fetch_from_git(
    vehicle_repo: str,
    vehicle_ref: str | None,
    motion_repo: str,
    motion_ref: str | None,
) -> Tuple[SourceBundle | None, SourceBundle | None]:
    """Fetch code bundles from Git repositories."""

    vehicle = _bundle_repo("vehicle", vehicle_repo.strip(), vehicle_ref.strip() if vehicle_ref else None)
    motion = _bundle_repo("motion", motion_repo.strip(), motion_ref.strip() if motion_ref else None)

    if vehicle is None and motion is None:
        raise ValueError("최소 한 개 이상의 Git 저장소를 입력하세요.")
    return vehicle, motion

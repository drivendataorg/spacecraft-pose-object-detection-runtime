from pathlib import Path

from loguru import logger
import pytest

SUBMISSION_PATH = Path("/submission/submission.csv")


def test_submission_exists():
    assert SUBMISSION_PATH.exists()

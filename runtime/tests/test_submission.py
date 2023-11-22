import os

from pathlib import Path

import pytest

SUBMISSION_PATH = Path("/code_execution/workdir/submission.csv")
CHECK_SUBMISSION = os.environ.get("CHECK_SUBMISSION", "false") == "true"


@pytest.mark.skipif(not CHECK_SUBMISSION, reason="Not checking submission yet")
def test_submission_exists():
    assert SUBMISSION_PATH.exists()

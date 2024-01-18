from pathlib import Path

import pandas as pd
from numpy.testing import assert_array_equal
from pandas._testing import assert_index_equal

SUBMISSION_PATH = Path("/code_execution/submission/submission.csv")
SUBMISSION_FORMAT_PATH = Path("/code_execution/data/submission_format.csv")


def test_submission_exists():
    assert SUBMISSION_PATH.exists(), f"Expected submission at {SUBMISSION_PATH}"


def test_submission_matches_submission_format():
    assert SUBMISSION_FORMAT_PATH.exists(), "Submission format not found"

    submission = pd.read_csv(SUBMISSION_PATH)
    fmt = pd.read_csv(SUBMISSION_FORMAT_PATH)

    assert_array_equal(submission.columns, fmt.columns, err_msg="Columns not identical")
    assert_index_equal(submission.index, fmt.index), "Index not identical"

    for col in submission.columns:
        assert submission[col].dtype == fmt[col].dtype, f"dtype for columns not equal"

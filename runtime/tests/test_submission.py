from pathlib import Path

import numpy as np
import pandas as pd
from numpy.testing import assert_array_equal
from pandas._testing import assert_index_equal

SUBMISSION_PATH = Path("/code_execution/submission/submission.csv")
SUBMISSION_FORMAT_PATH = Path("/code_execution/data/submission_format.csv")
MAX_WIDTH = 1280
MAX_HEIGHT = 1024


def test_submission_exists():
    assert SUBMISSION_PATH.exists(), f"Expected submission at {SUBMISSION_PATH}"


def test_submission_matches_submission_format():
    assert SUBMISSION_FORMAT_PATH.exists(), "Submission format not found"

    submission = pd.read_csv(SUBMISSION_PATH)
    fmt = pd.read_csv(SUBMISSION_FORMAT_PATH)
    assert_array_equal(submission.columns, fmt.columns, err_msg="Columns not identical")

    submission = submission.set_index("image_id")
    fmt = fmt.set_index("image_id")
    assert_index_equal(submission.index, fmt.index), "Index not identical"

    for col in submission.columns:
        assert submission[col].dtype == fmt[col].dtype, f"dtype for columns not equal"
        assert submission[col].notnull().all(), "Missing values found in submission"
        assert np.isfinite(submission[col]).all(), "Non-finite values found in submission"

    assert (submission["xmax"] <= MAX_WIDTH).all(), f"not all values of xmax are ≤ {MAX_WIDTH}"
    assert (submission["ymax"] <= MAX_HEIGHT).all(), f"not all values of ymax are ≤ {MAX_HEIGHT}"
    for min_col in ("xmin", "ymin"):
        assert (submission[min_col] >= 0).all(), f"not all values of {min_col} are non-negative"
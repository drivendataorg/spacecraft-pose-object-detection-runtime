import sys
from pathlib import Path

import pandas as pd

from main import COLUMN_NAMES

DATA_DIR = Path("/code_execution/data/")
SUBMISSION_FORMAT_PATH = DATA_DIR / "submission_format.csv"
SUBMISSION_DIR = Path("/code_execution/submission/")
OUTPUT_PATH = SUBMISSION_DIR / "submission.csv"


def main(partials_paths: list[Path]) -> pd.DataFrame:
    submission_df = pd.DataFrame(columns=COLUMN_NAMES)
    for csv_path in partials_paths:
        partial_df = pd.read_csv(csv_path)
        submission_df = pd.concat([submission_df, partial_df], axis="index")
    return submission_df.sort_values(by="image_id")


if __name__ == "__main__":
    partials_dir = Path(sys.argv[1])
    partials_paths = list(sorted(partials_dir.glob("*.csv")))
    print(
        f"assembling all predictions for {len(partials_paths):,} different partials ..."
    )
    df = main(partials_paths)
    df.to_csv(OUTPUT_PATH, index=False)
    print(f"wrote {len(df):,} lines of output to {OUTPUT_PATH}")

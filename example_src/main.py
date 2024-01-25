import os
from pathlib import Path

import click
import cv2
import numpy.typing as npt
import pandas as pd
from loguru import logger
from tqdm import tqdm

COLUMN_NAMES = ["image_id", "xmin", "ymin", "xmax", "ymax"]


def detect_object_in_image(img_arr: npt.ArrayLike) -> pd.Series:
    # TODO: actually make predictions! we don't actually do anything useful here!
    values = pd.Series(
        {
            "xmin": 10,
            "ymin": 10,
            "xmax": 20,
            "ymax": 20,
        }
    )
    return values


@click.command()
@click.argument(
    "data_dir",
    type=click.Path(exists=True, file_okay=False, dir_okay=True),
)
@click.argument(
    "output_path",
    type=click.Path(exists=False),
)
def main(data_dir, output_path):
    data_dir = Path(data_dir).resolve()
    output_path = Path(output_path).resolve()
    assert (
        output_path.parent.exists()
    ), f"Expected output directory {output_path.parent} does not exist"

    logger.info(f"using data dir: {data_dir}")
    assert data_dir.exists(), f"Data directory does not exist: {data_dir}"

    # read in the submission format
    submission_format_path = data_dir / "submission_format.csv"
    submission_format_df = pd.read_csv(submission_format_path, index_col="image_id")

    # copy over the submission format so we can overwrite placeholders with predictions
    submission_df = submission_format_df.copy()

    image_dir = data_dir / "images"

    # add a progress bar using tqdm without spamming the log
    update_iters = min(100, int(submission_format_df.shape[0] / 10))
    with open(os.devnull, "w") as devnull:
        progress_bar = tqdm(
            enumerate(submission_format_df.index.values),
            total=submission_format_df.shape[0],
            miniters=update_iters,
            file=devnull,
        )
        for i, image_id in progress_bar:
            if (i % update_iters) == 0:
                logger.info(str(progress_bar))
            image_path = image_dir / f"{image_id}.png"
            assert image_path.exists(), f"Expected image not found: {image_path}"
            # load the image
            img_arr = cv2.imread(str(image_path))
            box_series = detect_object_in_image(img_arr)
            submission_df.loc[image_id] = box_series

    submission_df.to_csv(output_path, index=True)


if __name__ == "__main__":
    main()

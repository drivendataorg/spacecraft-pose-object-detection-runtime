from pathlib import Path

import click
import cv2
import numpy as np
import pandas as pd
from loguru import logger
from tqdm import tqdm
from ultralytics import YOLO

INDEX_COLS = ["chain_id", "i"]
PREDICTION_COLS = ["x", "y", "z", "qw", "qx", "qy", "qz"]
REFERENCE_VALUES = [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0]


def centered_box(img, scale=0.1):
    """
    Return coordinates for a centered bounding box on the image, defaulting to 10% of the image's height and width.
    """
    # Get image dimensions
    height, width, _ = img.shape
    # Calculate the center of the image
    center_x, center_y = width // 2, height // 2
    # Calculate 10% of the image's height and width for the bounding box
    box_width, box_height = width * scale, height * scale
    # Calculate top-left corner of the bounding box
    x1 = center_x - box_width // 2
    y1 = center_y - box_height // 2
    # Calculate bottom-right corner of the bounding box
    x2 = center_x + box_width // 2
    y2 = center_y + box_height // 2

    return [x1, y1, x2, y2]


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
    assert output_path.parent.exists(), f"Expected output directory {output_path.parent} does not exist"

    logger.info(f"using data dir: {data_dir}")
    assert data_dir.exists(), f"Data directory does not exist: {data_dir}"

    # # read in the submission format
    # submission_format_path = data_dir / "submission_format.csv"
    # submission_format_df = pd.read_csv(submission_format_path)
    #
    # # copy over the submission format so we can overwrite placeholders with predictions
    # submission_df = submission_format_df.copy()

    image_dir = data_dir / "images"

    model = YOLO('yolov8n.pt')  # load pretrained model

    img_paths = list(image_dir.glob("*.png"))
    predictions = {}

    for img_path in tqdm(img_paths, total=len(img_paths)):
        # load the image
        img = cv2.imread(str(img_path))
        # got yolo result
        result = model(img, verbose=False)[0]
        # get bbox coordinates if they exist, otherwise just get a generic box in center of an image
        bbox = result.boxes.xyxy[0].tolist() if len(result.boxes) > 0 else centered_box(img)
        # store the result
        predictions[img_path.stem] = bbox

    submission_df = pd.DataFrame.from_dict(predictions, orient="index")
    submission_df.columns = ["xmin", "ymin", "xmax", "ymax"]

    submission_df.to_csv(output_path, index=True)


if __name__ == "__main__":
    main()

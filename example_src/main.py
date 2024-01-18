import sys
import time
from datetime import datetime
from pathlib import Path

import cv2
import pandas as pd

COLUMN_NAMES = ["image_id", "xmin", "ymin", "xmax", "ymax"]


def detect_object_in_image(img_path: str) -> dict:
    image_id = Path(img_path).stem
    # load the image
    _img = cv2.imread(img_path)
    # TODO: actually make predictions! we don't actually do anything useful here!
    values = {
        "image_id": image_id,
        "xmin": 10,
        "ymin": 10,
        "xmax": 20,
        "ymax": 20,
    }
    return values


if __name__ == "__main__":
    output_dir = Path(sys.argv[1])
    output_path = output_dir / f"{datetime.utcnow().isoformat()}.csv"

    # here is where we would load an expensive model, but it only happens once
    # for each chunk of image paths we're going to churn through
    time.sleep(1)

    results = []
    img_paths = [img_path.strip() for img_path in sys.stdin]
    print(
        f"making {len(img_paths):,} predictions for {img_paths[0]} to {img_paths[-1]}"
    )
    for img_path in img_paths:
        result = detect_object_in_image(img_path)
        results.append(result)
    df = pd.DataFrame.from_records(results, columns=COLUMN_NAMES)
    df.to_csv(output_path, index=False)

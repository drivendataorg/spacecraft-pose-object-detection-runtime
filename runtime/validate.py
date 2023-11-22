from pathlib import Path
import sys


if __name__ == "__main__":
    phase = sys.argv[1]
    working_dir = Path(sys.argv[2]).resolve()
    sys.exit(0)
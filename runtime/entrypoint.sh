#!/bin/bash

set -euxo pipefail

main () {
    expected_filename=main.sh

    cd /code_execution

    submission_files=$(zip -sf ./submission/submission.zip)
    if ! grep -q ${expected_filename}<<<$submission_files; then
        echo "Submission zip archive must include $expected_filename"
    return 1
    fi

    echo Unpacking submission
    unzip ./submission/submission.zip -d ./src

    echo Printing submission contents
    find src

    LOGURU_LEVEL=INFO sh main.sh

    pytest tests/test_submission
}

main |& tee "/code_execution/submission/log.txt"
exit_code=${PIPESTATUS[0]}

cp /code_execution/submission/log.txt /tmp/log

exit $exit_code
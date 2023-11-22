#!/bin/bash
set -e

exit_code=0

run_user_code() {
    if [ -f "main.sh" ]
    then
        echo "Running main.sh ..."
        sh main.sh
    else
        echo "ERROR: Could not find main.sh in submission.zip"
        exit_code=1
    fi
}

split () {
  echo "********************************************************************************"
}


{
    sleep 10
    source activate condaenv
    cd /submission

    echo "Unpacking submission..."
    unzip -o /submission/submission.zip -d ./
    python /validate.py pre_create .

    run_user_code
    python /validate.py post_query .

    echo "Exporting submission.csv result..."

    # Valid scripts must create a "submission.csv" file within the same directory as main
    if [ -f "submission.csv" ]
    then
        echo "Script completed its run."
    else
        echo "ERROR: Script did not produce a submission.csv file in the main directory."
        exit_code=1
    fi

    echo "Running acceptance tests..."
    # conda run -n condaenv --no-capture-output pytest --verbose --rootdir=. /tests/test_submission.py
    echo "================ END ================"
} |& tee "/submission/log.txt"

# copy for additional log uses
cp /submission/log.txt /tmp/log
exit $exit_code

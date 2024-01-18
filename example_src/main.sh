#!/usr/bin/env bash

# create a temporary directory to stash individual chain CSVs in
temp_dir=$(mktemp -d)

# find all the images in advance so we can parallelize in the next step
find ../data/images -maxdepth 1 -mindepth 1 -type f > images.txt

# use GNU parallel to run main.py on all of the chain dirs we just found
# - the argument to main.py is the temporary directory we just created
# - the argument -j 3 means use all 3 available cores
# - the argument --pipe means to split stdin (standard input) to multiple jobs.
# - the argument --ungroup means to ungroup the output (do it in any order)
# - the argument -L <N> means to do break the file list up into chunks of at most that many at a time
<images.txt parallel -j 3 --pipe -L1000 python main.py $temp_dir

# use our other script to put all these individual CSVs back together into the expected format
python assemble.py $temp_dir
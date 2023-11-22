### If you haven't already done so, start by reading the [Code Submission Format](https://www.drivendata.org/competitions/260/spacecraft-detection/page/835/) page on the competition website.


# Pose Bowl: Code execution runtime

Welcome to the runtime repository for the **Detection Track** of the [Pose Bowl: Spacecraft Detection and Pose Estimation Challenge](https://www.drivendata.org/competitions/260/spacecraft-detection/page/832/)!

This repository contains the definition of the environment where your code submissions will run. It specifies both the operating system and the software packages that will be available to your solution.

This repository has three primary uses for competitors:

💡 **Working example solutions** to help you get started with the challenge: 
- **[Quickstart example](https://github.com/drivendataorg/spacecraft-pose-object-detection-runtime/tree/main/example_src):** A minimal example that runs succesfully in the runtime environment and outputs a properly formatted submission CSV. This will generate arbitrary predictions, so unfortunately you won't win the competition with this example, but you can use it as a guide for bringing in your own work and generating a real submission.
- **[Benchmark example](https://github.com/drivendataorg/spacecraft-pose-object-detection-runtime/tree/main/benchmark):**  A modestly more advanced example that uses a YOLO pretrained model to generate bounding box predictions. You won't win the competition with this example either, but it's a step in the right direction.

🔧 **Test your submission**: Test your submission with a locally running version of the container to discover errors before submitting to the competition site. Generally, this should save you time and help you iterate faster.


📦 **Request new packages in the official runtime**: All packages required by your submission must be pre-installed, and your submission will not have internet access. If you want to use a package that is not in the runtime environment, make a pull request to this repository.


 ----

### [Quickstart](#quickstart)
 - [Prerequisites](#prerequisites)
 - [Download the data](#download-the-data)
 - [The quickstart example](#the-quickstart-example)
 - [Testing the submission](#testing-the-submission)
 - [Evaluating locally](#evaluating-locally)
### [Developing your own submission](#developing-your-own-submission)
 - [Steps](#steps)
 - [Logging](#logging)
### [Additional information](#additional-information)
 - [Runtime network access](#runtime-network-access)
 - [Downloading pre-trained weights](#downloading-pre-trained-weights)
 - [Updating runtime packages](#updating-runtime-packages)


----

## Quickstart

This section guides you through the steps to test a simple but valid submission for the competition.

### Prerequisites

First, make sure you have the prerequisites installed.

 - A clone or fork of this repository
 - Enough free space on your machine for the spacecraft images dataset (at least 10 GB) and Docker container images (5 GB)
 - [Docker](https://docs.docker.com/get-docker/)
 - [GNU make](https://www.gnu.org/software/make/) (optional, but useful for running commands in the Makefile)

### Download the data

First, go to the challenge [download page](https://www.drivendata.org/competitions/260/spacecraft-detection/data/) to start downloading the challenge data. Save the `submission_format.csv` and `training_labels.csv` in this project's `/data` directory.

The images dataset is broken up into individual tar files of approximately 10 GB in size. Download at least one of these tar files to get started, and then extract it to the `data/images` directory.

Once everything is downloaded and in the right location, it should look something like this:

```
data/                         # Runtime data directory
├── images/                   # Directory containing image files
│      │
│      ├── a0a0d73d0e9a4b16a23bc210a264fd3f.png
│      ├── a0a6efb87e1fcd0c158ba35ced823250.png
│      ├── a0a0d73d0e9a4b16a23bc210a264fd3f.png
│      ├── a0a6efb87e1fcd0c158ba35ced823250.png
│      └── ...
│
├── submission_format.csv     # CSV file showing how submission should be formatted
└── train_labels.csv          # CSV file with ground truth data
```

Later in this guide, when we launch a Docker container from your computer (or the "host" machine), the `data` directory on your host machine will be mounted as a read-only directory in the container as `/code_execution/data`. In the runtime, your code will then be able to access all the competition data at `/code_execution/data`, which will by default look to your script like `./data` since your script will be invoked with `/code_execution` as the working directory.

### The quickstart example

A working example submission is provided in this project's [`example_src/`](https://github.com/drivendataorg/spacecraft-pose-object-detection-runtime/tree/main/example_src) to help you get started. 

In that directory, you'll see the [`main.sh`](https://github.com/drivendataorg/spacecraft-pose-object-detection-runtime/blob/main/example_src/main.sh) file that you're required to include in your submission. Below are the full contents of that file -- for this simple example, this script simply sets a couple path variables and runs a python script called `main.py`. You are welcome to make the `main.sh` behavior more complex as you develop your submission and there's no requirement that you use it to call a python script (we just think this will be a fairly common use pattern).

```bash
#!/usr/bin/env bash

DATA_DIR=/code_execution/data
SUBMISSION_PATH=/code_execution/submission/submission.csv

# call our script (main.py in this case) and tell it where the data and submission live
python main.py $DATA_DIR $SUBMISSION_PATH
```

The [`main.py`](https://github.com/drivendataorg/spacecraft-pose-object-detection-runtime/blob/main/example_src/main.py) script is fairly straightforward as well. For this quickstart example, the script doesn't even try to generate reasonable predictions. It just returns an arbitrary bounding box for each image in the dataset. That won't generate a very good score, but it will still be a valid submission, which is what we're interested in for starters.

### Testing the submission

The primary purpose of this runtime repository is to allow you to easily test your submission before making a submission to the DrivenData platform. 

Your submission is going to run inside a Docker container on our code execution platform. This repository contains the definition for that (Docker container)[https://github.com/drivendataorg/spacecraft-pose-object-detection-runtime/tree/main/runtime], as well as a few commands you can run to easily download the Docker image and test your submission. Below we walk through those commands.

First, make sure Docker is running and then run the following commands in your terminal:

1. **`make pull`** downloads the latest official Docker image from the container registry ([Azure](https://azure.microsoft.com/en-us/services/container-registry/)). You'll need an internet connection for this.
2. **`make pack-example`** zips the contents of the `example_src` directory and saves it as `submission/submission.zip`. This is the file that you will upload to the DrivenData competition site for code execution. But first we'll test that everything looks good locally in step #3. 
   * Note: When running this again in the future, you may need to first run `make clean` before you re-pack the example for submission, both because it won't rerun by default if the submission file already exists, and also because sometimes running with Docker before may have created files in the mounted submission directory with different permissions.
3. **`make test-submission`** will do a test run of your submission, simulating what happens during actual code execution. This command runs the Docker container with the requisite host directories mounted, and executes `main.sh` to produce a CSV file with your image rankings at `submission/submission.csv`.

```sh
make pull
make clean && make pack-example
make test-submission
```

🎉 **Congratulations!** You've just tested a submission for the Pose Bowl challenge. If everything worked as expected, you should see a new file has been generated at `submission/submission.csv`.

If you were ready to make a real submission to the competition, you would upload the `submission.zip` file from step 2 above to the competition [Submissions page](https://www.drivendata.org/competitions/260/spacecraft-detection/submissions/). Once submitted, our code execution platform would then run your submission, and generate a new `submission.csv` on the unseen test set that will get **scored** automatically using the [competition scoring metric](https://www.drivendata.org/competitions/260/spacecraft-detection/page/833/#performance-metric) to determine your rank on the leaderboard.

### Evaluating locally

In your local model development and cross validation, you may wish to use the same scoring
metric that will be employed when your real submissions are scored. We have included a script
that implements the same logic at `scripts/score.py`.

The usage is:

```
❯ python scripts/score.py --help
usage: score.py [-h] predicted_path actual_path

Calculates the Jaccard index score for the Pose Bowl: Spacecraft Detection and Pose Estimation Challenge. Args: predicted_path (str | Path): Path to predictions CSV file matching submission format
actual_path (str | Path): Path to ground truth CSV file Returns: Dict[str, float]: Jaccard index score

positional arguments:
  predicted_path  Path to predictions CSV.
  actual_path     Path to ground truth CSV.

options:
  -h, --help      show this help message and exit
```

For example, using the `submission_format.py` as the predictions with our training labels as the
ground truth, we can verify that we achieve a (bad!) score:

```
❯ python scripts/score.py data/submission_format.csv data/train_labels.csv 
{
  "score": 0.0
}
```


----

## Developing your own submission

Now that you've gone through the quickstart example, let's talk about how to develop your own solution for the competition.

### Steps

This section provides instructions on how to develop and run your code submission locally using the Docker container. To make things simpler, key processes are already defined in the `Makefile`. Commands from the `Makefile` are then run with `make {command_name}`. The basic steps are:

```
make pull
make pack-submission
make test-submission
```

Let's walk through what you'll need to do, step-by-step. The overall process here is very similar to what we've already covered in the [Quickstart](#quickstart), but we'll go into more depth this time around.

1. **[Set up the prerequisites](#prerequisites)**

2. **[Download the data](#download-the-data)**

3. **Download the official competition Docker image:**

    ```bash
    $ make pull
    ```

4. ⚙️ **Save all of your submission files, including the required `main.sh` script, in the `submission_src` folder of the runtime repository.** This is where the real work happens.
   * You are free to modify the `main.sh` scripts we've provided as examples. Just make sure that you adhere to the competition rules and you still produce a `submission.csv` in the correct format.
   * Also keep in mind that the runtime already contains a number of packages that might be useful for you (see: [environment.yml](https://github.com/drivendataorg/spacecraft-pose-object-detection-runtime/blob/main/runtime/environment.yml)). If there are other packages you'd like added, see the section below on [updating runtime packages](#updating-runtime-packages).
   * Finally, make sure any model weights or other files you need are also saved in `submission_src`.

5. **Create a `submission/submission.zip` file containing your code and model assets in `submission_src`:**

    ```bash
    $ make pack-submission
    ```

6. **Test your submission.** The command below will launch an instance of the competition Docker image, replicating the same inference process that takes place in the official code execution runtime. This will mount the requisite host directories on the Docker container, unzip `submission/submission.zip` into the root directory of the container, and then execute `main.sh` to produce a CSV file with your predictions at `submission/submission.csv`.

   ```
   $ make test-submission
   ```


> ⚠️ **Remember** that for local testing purposes, the `/code_execution/data` directory is just a mounted version of what you have saved locally in this project's `data` directory. So you will just be using the publicly available training files for local testing. In the official code execution environment, `/code_execution/data` will contain the _actual test data_, which no participants have access to, and this is what will be used to compute your score for the leaderboard.


### Logging

When you run `make test-submission` the logs will be printed to the terminal and written out to `submission/log.txt`. If you run into errors, use the `log.txt` to determine what changes you need to make for your code to execute successfully. This same log will be kept when you make a submission on the platform, which you can access through the user interface. Note: try to be judicious about what you keep in the log - if the log is overly chatty it may get truncated when you view it on the platform.


---
## Additional information

### Runtime network access

All internet access is blocked in the runtime environment. This means that you will need to package any required resources into your `submission.zip`. 

For example, it is common for models to download pre-trained weights. Since submissions do not have internet access, you will need to include all weights along with your `submission.zip` and make sure that your code loads them from disk and rather than the internet.


### Updating runtime packages

If you want to use a package that is not in the environment, you are welcome to make a pull request to this repository. If you're new to the GitHub contribution workflow, check out [this guide by GitHub](https://docs.github.com/en/get-started/quickstart/contributing-to-projects). The runtime manages dependencies using [conda](https://docs.conda.io/en/latest/) environments. [Here is a good general guide](https://towardsdatascience.com/a-guide-to-conda-environments-bc6180fc533) to conda environments. The official runtime uses **Python 3.10** environment.

To submit a pull request for a new package:

1. Fork this repository.

2. Edit the [conda](https://docs.conda.io/en/latest/) environment YAML file, `runtime/environment.yml`. There are two ways to add a requirement:
    - Add an entry to the `dependencies` section. This installs from a conda channel using `conda install`. Conda performs robust dependency resolution with other packages in the `dependencies` section, so we can avoid package version conflicts.
    - Add an entry to the `pip` section. This installs from PyPI using `pip`, and is an option for packages that are not available in a conda channel.

    For both methods be sure to include a version, e.g., `numpy==1.20.3`. This ensures that all environments will be the same.

3. Locally test that the Docker image builds successfully:

    ```sh
    make build
    ```

4. Commit the changes to your forked repository.

5. Open a pull request from your branch to the `main` branch of this repository. Navigate to the [Pull requests](https://github.com/drivendataorg/spacecraft-pose-object-detection-runtime/pulls) tab in this repository, and click the "New pull request" button. For more detailed instructions, check out [GitHub's help page](https://help.github.com/en/articles/creating-a-pull-request-from-a-fork).

6. Once you open the pull request, Github Actions will automatically try building a new Docker image with your changes and running the tests in `runtime/tests`. These tests can take up to 30 minutes, and may take longer if your build is queued behind others. You will see a section on the pull request page that shows the status of the tests and links to the logs.

7. You may be asked to submit revisions to your pull request if the tests fail or if a DrivenData team member has feedback. Pull requests won't be merged until all tests pass and the team has reviewed and approved the changes.

---

## Good luck! And have fun!

Thanks for reading! Enjoy the competition, and [hit up the forums](tktk) if you have any questions!

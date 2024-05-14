#!/usr/bin/env bash

DATA_DIR=/code_execution/data
SUBMISSION_PATH=/code_execution/submission/submission.csv

# call our script (main.py in this case) and tell it where the data and submission live
cd yolov9

#Compile the project
g++ -std=c++17 -lstdc++fs -o yolo_ort \
    -Iinclude/ \
    -I ../onnxruntime-linux-x64-1.12.0/include \
    -I ../opencv-4.9.0/include \
    -I ../opencv-4.9.0/build \
    -I ../opencv-4.9.0/modules/calib3d/include \
    -I ../opencv-4.9.0/modules/dnn/include \
    -I ../opencv-4.9.0/modules/core/include \
    -I ../opencv-4.9.0/modules/features2d/include \
    -I ../opencv-4.9.0/modules/flann/include \
    -I ../opencv-4.9.0/modules/highgui/include \
    -I ../opencv-4.9.0/modules/gapi/include \
    -I ../opencv-4.9.0/modules/imgcodecs/include \
    -I ../opencv-4.9.0/modules/imgproc/include \
    -I ../opencv-4.9.0/modules/ml/include \
    -I ../opencv-4.9.0/modules/objdetect/include \
    -I ../opencv-4.9.0/modules/photo/include \
    -I ../opencv-4.9.0/modules/stitching/include \
    -I ../opencv-4.9.0/modules/video/include \
    -I ../opencv-4.9.0/modules/ts/include \
    -I ../opencv-4.9.0/modules/videoio/include \
    src/detector.cpp \
    src/utils.cpp \
    src/main.cpp \
    -L ../onnxruntime-linux-x64-1.12.0/lib \
    -L ../opencv-4.9.0/build/lib \
    -Wl,-rpath,../onnxruntime-linux-x64-1.12.0/lib \
    -Wl,-rpath,../opencv-4.9.0/build/lib \
    -l onnxruntime \
    -l opencv_calib3d \
    -l opencv_dnn \
    -l opencv_core \
    -l opencv_features2d \
    -l opencv_flann \
    -l opencv_highgui \
    -l opencv_gapi \
    -l opencv_imgcodecs \
    -l opencv_imgproc \
    -l opencv_ml \
    -l opencv_objdetect \
    -l opencv_photo \
    -l opencv_stitching \
    -l opencv_video \
    -l opencv_ts \
    -l opencv_videoio \

#Run the project
./yolo_ort -i $DATA_DIR/images -o $SUBMISSION_PATH

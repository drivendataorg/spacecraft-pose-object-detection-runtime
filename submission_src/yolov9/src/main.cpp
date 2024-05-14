#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>

#include "cmdline.h"
#include "utils.h"
#include "detector.h"


#define OUTPUT_FILE "../../../submission.csv"
#define IMAGE_DIRECTORY "../../../data/images"


int main(int argc, char* argv[])
{

    //************************COMMAND LINE SETUP************************//
    const float confThreshold = 0.3f;
    const float iouThreshold = 0.3f;
    std::vector<std::filesystem::path> imagePaths;

    
    cmdline::parser cmd;
    cmd.add<std::string>("model_path", 'm', "Path to onnx model.", false, "models/quang_yolov9.onnx");
    cmd.add<std::string>("image_directory", 'i', "Image directory.", false, IMAGE_DIRECTORY);
    cmd.add<std::string>("class_names", 'c', "Path to class names file.", false, "models/spacecraft.names");
    cmd.add<std::string>("output", 'o', "Output file.", false, OUTPUT_FILE);
    cmd.add("gpu", '\0', "Inference on cuda device.");
    cmd.add("visualization", 'v', "Enable visualization.");

    cmd.parse_check(argc, argv);

    bool isGPU = cmd.exist("gpu");
    const std::string classNamesPath = cmd.get<std::string>("class_names");
    const std::vector<std::string> classNames = utils::loadNames(classNamesPath);
    const std::string imageDirectoryPath = cmd.get<std::string>("image_directory");
    const std::string modelPath = cmd.get<std::string>("model_path");
    const std::string output = cmd.get<std::string>("output");
    bool enableVisualization = cmd.exist("visualization");

    std::ofstream file(output);
    //Write first row
    file << "image_id,xmin,ymin,xmax,ymax" << std::endl;
    

    if (classNames.empty())
    {
        std::cerr << "Error: Empty class names file." << std::endl;
        return -1;
    }


    //Image batches
    utils::createImageBatch(imageDirectoryPath, imagePaths);

    //************************DETECTOR SETUP************************//
    YOLODetector detector {nullptr};
    cv::Mat image;
    std::vector<Detection> result;

    std::cout << "Starting inference..." << std::endl;
    std::cout << "Number of images: " << imagePaths.size() << std::endl;
    detector = YOLODetector(modelPath, isGPU, cv::Size(640, 640));
    std::cout << "Model was initialized." << std::endl;

    int numDetected = 0;
    for (const auto& imagePath : imagePaths) {
        try
        {
            image = cv::imread(imagePath.string());

            //A vector of detection objects
            result = detector.detect(image, confThreshold, iouThreshold);
            utils::writeToCSV(result, imagePath.filename().string(), file);
            numDetected++;
            std::cout << numDetected << "/" << imagePaths.size() << " images processed." << std::endl;
            //Visualizing detection
            if (enableVisualization) {
                utils::visualizeDetection(image, result, classNames);

                //Opens the window
                cv::imshow("result", image);
                // cv::imwrite("result.jpg", image);
                cv::waitKey(0);
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            continue;
        }
    }

    return 0;
}

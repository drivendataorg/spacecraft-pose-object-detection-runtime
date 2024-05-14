#pragma once
#include <codecvt>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <filesystem>


struct Detection
{
    cv::Rect box;
    float conf{};
    int classId{};
};

namespace utils
{   
    //Reads all images from a directory and stores their paths in a vector
    void createImageBatch(const std::string& imageDir, std::vector<std::filesystem::path>& imagePaths);

    //Writes the detection results to a CSV file
    void writeToCSV(std::vector<Detection> result, std::string imageID, std::ofstream &ostream);
    size_t vectorProduct(const std::vector<int64_t>& vector);
    std::wstring charToWstring(const char* str);
    std::vector<std::string> loadNames(const std::string& path);
    void visualizeDetection(cv::Mat& image, std::vector<Detection>& detections,
                            const std::vector<std::string>& classNames);

    void letterbox(const cv::Mat& image, cv::Mat& outImage,
                   const cv::Size& newShape,
                   const cv::Scalar& color,
                   bool auto_,
                   bool scaleFill,
                   bool scaleUp,
                   int stride);

    void scaleCoords(const cv::Size& imageShape, cv::Rect& box, const cv::Size& imageOriginalShape);

    template <typename T>
    T clip(const T& n, const T& lower, const T& upper);
}

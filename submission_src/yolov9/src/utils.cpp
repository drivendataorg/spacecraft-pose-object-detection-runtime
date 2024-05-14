#include "utils.h"


void utils::createImageBatch(const std::string& imageDir, std::vector<std::filesystem::path>& imagePaths)
{
    for (const auto& entry : std::filesystem::directory_iterator(imageDir))
    {
        if (entry.path().extension() == ".png")
            imagePaths.emplace_back(entry.path());
    }



}

void utils::writeToCSV(std::vector<Detection> result, std::string imageID, std::ofstream &ostream) {
    //Get max confidence detection
    Detection detection;
    float maxConf = 0;
    if (result.empty()) {
        ostream << imageID.substr(0, imageID.size() - 4) << std::endl;
        ostream << 0 << "," << 0 << "," << 0 << "," << 0 << std::endl;
        return;
    }
    for (int i = 0; i < result.size(); i++) {
        if (result[i].conf > maxConf) {
            detection = result[i];
            maxConf = result[i].conf;
        }
    }
    //Convert the box to min-max format
    int max_X = detection.box.x + detection.box.width;
    int min_X = detection.box.x;
    int max_Y = detection.box.y + detection.box.height;
    int min_Y = detection.box.y;

    //Write to CSV
    ostream << imageID.substr(0, imageID.size() - 4) << ",";
    ostream << ((min_X>0)?min_X:0) << ",";
    ostream << ((min_Y>0)?min_Y:0) << ",";
    ostream << ((max_X<1024 && max_X>min_X)?max_X:min_X) << ",";
    ostream << ((max_Y<1024 && max_Y>min_Y)?max_Y:min_Y) << std::endl;

}

size_t utils::vectorProduct(const std::vector<int64_t>& vector)
{
    if (vector.empty())
        return 0;

    size_t product = 1;
    for (const auto& element : vector)
        product *= element;

    return product;
}

std::wstring utils::charToWstring(const char* str)
{
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.from_bytes(str);
}

std::vector<std::string> utils::loadNames(const std::string& path)
{
    // load class names
    std::vector<std::string> classNames;
    std::ifstream infile(path);
    if (infile.good())
    {
        std::string line;
        while (getline (infile, line))
        {
            if (line.back() == '\r')
                line.pop_back();
            classNames.emplace_back(line);
        }
        infile.close();
    }
    else
    {
        std::cerr << "ERROR: Failed to access class name path: " << path << std::endl;
    }

    return classNames;
}


void utils::visualizeDetection(cv::Mat& image, std::vector<Detection>& detections,
                               const std::vector<std::string>& classNames)
{
    for (const Detection& detection : detections)
    {
        cv::rectangle(image, detection.box, cv::Scalar(229, 160, 21), 2);

        int x = detection.box.x;
        int y = detection.box.y;

        int conf = (int)std::round(detection.conf * 100);
        int classId = detection.classId;
        std::string label = classNames[classId] + " 0." + std::to_string(conf);

        int baseline = 0;
        cv::Size size = cv::getTextSize(label, cv::FONT_ITALIC, 0.8, 2, &baseline);
        cv::rectangle(image,
                      cv::Point(x, y - 25), cv::Point(x + size.width, y),
                      cv::Scalar(229, 160, 21), -1);

        cv::putText(image, label,
                    cv::Point(x, y - 3), cv::FONT_ITALIC,
                    0.8, cv::Scalar(255, 255, 255), 2);
    }
}

//Resizes the image in place, maintains aspect ratio
void utils::letterbox(const cv::Mat& image, cv::Mat& outImage,
                      const cv::Size& newShape = cv::Size(640, 640),
                      const cv::Scalar& color = cv::Scalar(114, 114, 114),
                      bool auto_ = true,
                      bool scaleFill = false,
                      bool scaleUp = true,
                      int stride = 32)
{
    cv::Size shape = image.size();
    float r = std::min((float)newShape.height / (float)shape.height,
                       (float)newShape.width / (float)shape.width);
    if (!scaleUp)
        r = std::min(r, 1.0f);

    float ratio[2] {r, r};
    int newUnpad[2] {(int)std::round((float)shape.width * r),
                     (int)std::round((float)shape.height * r)};

    auto dw = (float)(newShape.width - newUnpad[0]);
    auto dh = (float)(newShape.height - newUnpad[1]);

    if (auto_)
    {
        dw = (float)((int)dw % stride);
        dh = (float)((int)dh % stride);
    }
    else if (scaleFill)
    {
        dw = 0.0f;
        dh = 0.0f;
        newUnpad[0] = newShape.width;
        newUnpad[1] = newShape.height;
        ratio[0] = (float)newShape.width / (float)shape.width;
        ratio[1] = (float)newShape.height / (float)shape.height;
    }

    dw /= 2.0f;
    dh /= 2.0f;

    if (shape.width != newUnpad[0] && shape.height != newUnpad[1])
    {
        cv::resize(image, outImage, cv::Size(newUnpad[0], newUnpad[1]));
    }

    int top = int(std::round(dh - 0.1f));
    int bottom = int(std::round(dh + 0.1f));
    int left = int(std::round(dw - 0.1f));
    int right = int(std::round(dw + 0.1f));
    cv::copyMakeBorder(outImage, outImage, top, bottom, left, right, cv::BORDER_CONSTANT, color);
}

void utils::scaleCoords(const cv::Size& imageShape, cv::Rect& coords, const cv::Size& imageOriginalShape)
{
    float gain = std::min((float)imageShape.height / (float)imageOriginalShape.height,
                          (float)imageShape.width / (float)imageOriginalShape.width);

    int pad[2] = {(int) (( (float)imageShape.width - (float)imageOriginalShape.width * gain) / 2.0f),
                  (int) (( (float)imageShape.height - (float)imageOriginalShape.height * gain) / 2.0f)};

    coords.x = (int) std::round(((float)(coords.x - pad[0]) / gain));
    coords.y = (int) std::round(((float)(coords.y - pad[1]) / gain));

    coords.width = (int) std::round(((float)coords.width / gain));
    coords.height = (int) std::round(((float)coords.height / gain));

    // // clip coords, should be modified for width and height
     coords.x = utils::clip(coords.x, 0, imageOriginalShape.width);
     coords.y = utils::clip(coords.y, 0, imageOriginalShape.height);
     coords.width = utils::clip(coords.width, 0, imageOriginalShape.width);
     coords.height = utils::clip(coords.height, 0, imageOriginalShape.height);
}

template <typename T>
T utils::clip(const T& n, const T& lower, const T& upper)
{
    return std::max(lower, std::min(n, upper));
}


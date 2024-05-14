#include "detector.h"
#include<algorithm>

//******************* EDIT POST PROCESSING TO HANDLE YOLOv9 OUTPUT TENSORS *******************//
YOLODetector::YOLODetector(const std::string& modelPath,
                           const bool& isGPU = true,
                           const cv::Size& inputSize = cv::Size(640, 640))
{
    //Sets up the environment with default session options and env
    env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "ONNX_DETECTION");
    sessionOptions = Ort::SessionOptions();

    std::vector<std::string> availableProviders = Ort::GetAvailableProviders();
    auto cudaAvailable = std::find(availableProviders.begin(), availableProviders.end(), "CUDAExecutionProvider");
    OrtCUDAProviderOptions cudaOption;


    //Looks for CPU/GPU execution providers - can be changed
    if (isGPU && (cudaAvailable == availableProviders.end()))
    {
        std::cout << "GPU is not supported by your ONNXRuntime build. Fallback to CPU." << std::endl;
        std::cout << "Inference device: CPU" << std::endl;
    }
    else if (isGPU && (cudaAvailable != availableProviders.end()))
    {
        std::cout << "Inference device: GPU" << std::endl;
        sessionOptions.AppendExecutionProvider_CUDA(cudaOption);
    }
    else
    {
        std::cout << "Inference device: CPU" << std::endl;
    }

#ifdef _WIN32
    std::wstring w_modelPath = utils::charToWstring(modelPath.c_str());
    session = Ort::Session(env, w_modelPath.c_str(), sessionOptions);
#else
    //Initializes the session
    session = Ort::Session(env, modelPath.c_str(), sessionOptions);
#endif
    
    //Memory allocator
    Ort::AllocatorWithDefaultOptions allocator;

    //Gets the type info of the input from the session 
    Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);

    //Gets the shape from the inputTypeInfo struct made above
    std::vector<int64_t> inputTensorShape = inputTypeInfo.GetTensorTypeAndShapeInfo().GetShape();

    this->isDynamicInputShape = false;

    // checking if width and height are dynamic
    if (inputTensorShape[2] == -1 && inputTensorShape[3] == -1)
    {
        std::cout << "Dynamic input shape" << std::endl;
        this->isDynamicInputShape = true;
    }

    //CHANGING THIS TO BE MORE CLEAR
    /* for (auto shape : inputTensorShape)
        std::cout << "Input shape: " << shape << std::endl;*/
    
    //OUTPUTTING INPUT SHAPE
    
    //Gets the name of the input and output from the session
    inputNames.push_back(session.GetInputName(0, allocator));
    outputNames.push_back(session.GetOutputName(0, allocator));

    this->inputImageShape = cv::Size2f(inputSize);
}

//From an iterator pointing to output tensor, gets the boxes and obj confidences. Gets the best out of these.
void YOLODetector::getBestClassInfo(std::vector<float>::iterator it, const int& numClasses,
                                    float& bestConf, int& bestClassId)
{

    // first 5 element are box and obj confidence
    bestClassId = 5;
    bestConf = 0;

    for (int i = 5; i < numClasses + 5; i++)
    {
        if (it[i] > bestConf)
        {
            bestConf = it[i];
            bestClassId = i - 5;
        }
    }

}

void YOLODetector::preprocessing(cv::Mat &image, float*& blob, std::vector<int64_t>& inputTensorShape)
{
    //CV Matrix for images
    cv::Mat resizedImage, floatImage;
    //Converts BGR to RGB - stored in resizedImage
    cv::cvtColor(image, resizedImage, cv::COLOR_BGR2RGB);
    //Resizes the image to the input size
    utils::letterbox(resizedImage, resizedImage, this->inputImageShape,
                     cv::Scalar(114, 114, 114), this->isDynamicInputShape,
                     false, true, 32);

    //Sets the input tensor shape (input tensor passed by reference)
    inputTensorShape[2] = resizedImage.rows;
    inputTensorShape[3] = resizedImage.cols;

    //Converts the resized image to float
    resizedImage.convertTo(floatImage, CV_32FC3, 1 / 255.0);
    //Bunch of image info in one vector
    blob = new float[floatImage.cols * floatImage.rows * floatImage.channels()];
    cv::Size floatImageSize {floatImage.cols, floatImage.rows};

    // hwc -> chw I guess it wasn't already in chw?
    std::vector<cv::Mat> chw(floatImage.channels());
    for (int i = 0; i < floatImage.channels(); ++i)
    {
        //Stores three images
        chw[i] = cv::Mat(floatImageSize, CV_32FC1, blob + i * floatImageSize.width * floatImageSize.height);
    }
    //Divides multi-channel floatImage into several single-channels chw
    cv::split(floatImage, chw);
}

std::vector<Detection> YOLODetector::postprocessing(const cv::Size& resizedImageShape,
                                                    const cv::Size& originalImageShape,
                                                    std::vector<Ort::Value>& outputTensors,
                                                    const float& confThreshold, const float& iouThreshold)
{
    std::vector<cv::Rect> boxes;
    std::vector<float> confs;
    std::vector<int> classIds;

    //Raw output tensors
    auto* rawOutput = outputTensors[0].GetTensorData<float>();

    //[batch, channels, height, width]
    std::vector<int64_t> outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();

    //total number of elements (multiplying all the elements in the shape vector)
    size_t count = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();

    //Initialize a vector of size count with the values of rawOutput
    std::vector<float> output(rawOutput, rawOutput + count);

    //NOTE THIS OUTPUT SHAPE
    // first 5 elements are box[4] and obj confidence
    //ONLY ONE CLASS IS IN THIS CASE
    int numClasses = 1;
    int elementsInBatch = (int)(outputShape[1] * outputShape[2]);
    
    // only for batch size = 1
    //Iterating over output vector

    /*
    int i = 0;
    for (i = 0; i < (42000-8400); i+=4) { 
        int centerX = output[i];
        int centerY = output[i+1];
        int width = output[i+2];
        int height = output[i+3];
        std::cout << centerX << " " << centerY << " " << width << " " << height << " " << std::endl;
        
    }

    for (i; i < 42000; i+=1) {
       float clsConf = output[i];
        std::cout << clsConf << " ";
    }
    */

   for (int i = 0; i < outputShape[2]; i++) {
    
        int centerX = output[i];
        int centerY = output[i+8400];
        int width = output[i+2*8400];
        int height = output[i+3*8400];
        float clsConf = output[i+4*8400];
        if (clsConf > confThreshold)
        {
            int left = centerX - width / 2;
            int top = centerY - height / 2;

            float objConf;
            int classId = 1;

            float confidence = clsConf;

            boxes.emplace_back(left, top, width, height);
            confs.emplace_back(confidence);
            classIds.emplace_back(classId);
        }
   }
    
    
    
    /*for (auto it = output.begin(); it != output.begin() + elementsInBatch; it += outputShape[1])
    {
        //Object 
        float clsConf = it[4];
        
        // if object confidence is less than threshold, skip
        if (clsConf > confThreshold)
        {
            // box[0] = center x, box[1] = center y, box[2] = w, box[3] = h
            std::cout << it[0] << " " << it[1] << " " << it[2] << " " << it[3] << " " << it[4] << std::endl;
            int centerX = (int) (it[0]);
            int centerY = (int) (it[1]);
            int width = (int) (it[2]);
            int height = (int) (it[3]);
            int left = centerX - width / 2;
            int top = centerY - height / 2;

            float objConf;
            int classId;
            this->getBestClassInfo(it, numClasses, objConf, classId);

            float confidence = clsConf;

            boxes.emplace_back(left, top, width, height);
            confs.emplace_back(confidence);
            classIds.emplace_back(classId);
        }
    }*/
    
    

    //Non-maximum suppression - filters out the unncessary bounding boxes
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confs, confThreshold, iouThreshold, indices);
    std::cout << "amount of NMS indices: " << indices.size() << std::endl;

    //Initializes a vector of detections 
    std::vector<Detection> detections;

    //In indices, each element is an index to boxes for a bounding box
    for (int idx : indices)
    {
        Detection det;
        //det.box contains the boxes
        det.box = cv::Rect(boxes[idx]);
        //Rescaling coordinates of bounding box
        utils::scaleCoords(resizedImageShape, det.box, originalImageShape);

        //det.conf contains the confidence
        det.conf = confs[idx];
        if (det.conf < confThreshold || det.conf == 0.0)
            continue;
        //det.classId contains the identified class
        det.classId = classIds[idx];
        std::cout << det.box << " " << det.conf << " " << det.classId << std::endl;
        //Placed into the detections array
        detections.emplace_back(det);
    }

    return detections;
}

//Public - producing an inference and returning a vector of detections
std::vector<Detection> YOLODetector::detect(cv::Mat &image, const float& confThreshold = 0.4,
                                            const float& iouThreshold = 0.45)
{
    //Initialize
    float *blob = nullptr;
    std::vector<int64_t> inputTensorShape {1, 3, -1, -1};
    //Image preprocess (why is inputTensorShape -1 -1?)
    this->preprocessing(image, blob, inputTensorShape);

    size_t inputTensorSize = utils::vectorProduct(inputTensorShape);

    std::vector<float> inputTensorValues(blob, blob + inputTensorSize);

    //Creates a vector of input tensors
    std::vector<Ort::Value> inputTensors;

    //Memory allocator
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(
            OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

    //CONFUSED ON THIS
    inputTensors.push_back(Ort::Value::CreateTensor<float>(
            memoryInfo, inputTensorValues.data(), inputTensorSize,
            inputTensorShape.data(), inputTensorShape.size()
    ));

    //Returns a vector of output tensors - requires input names, input tensors, output names, and run options
    std::vector<Ort::Value> outputTensors = this->session.Run(Ort::RunOptions{nullptr},
                                                              inputNames.data(),
                                                              inputTensors.data(),
                                                              1,
                                                              outputNames.data(),
                                                              1);

    //Resized shape is the size of the input tensor after preprocessing
    cv::Size resizedShape = cv::Size((int)inputTensorShape[3], (int)inputTensorShape[2]);
    std::vector<Detection> result = this->postprocessing(resizedShape,
                                                         image.size(),
                                                         outputTensors,
                                                         confThreshold, iouThreshold);

    delete[] blob;

    for (auto& det: result)
    {
        //std::cout << "Detection: " << det.box << " " << det.conf << " " << det.classId << std::endl;
    }
    return result;
}

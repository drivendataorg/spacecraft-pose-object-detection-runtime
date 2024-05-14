cmake_minimum_required(VERSION 3.25.1)
project(yolo_ort)

set(ONNXRUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../onnxruntime-linux-x64-1.12.0")

find_package(OpenCV REQUIRED)

include_directories("include/")

add_executable(yolo_ort
               src/main.cpp
               src/detector.cpp
               src/utils.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(yolo_ort PRIVATE "${ONNXRUNTIME_DIR}/include")
# link_directories("${ONNXRUNTIME_DIR}/lib")
target_compile_features(yolo_ort PRIVATE cxx_std_17)
target_link_libraries(yolo_ort ${OpenCV_LIBS})

target_link_libraries(yolo_ort "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so")
target_link_libraries(yolo_ort "${ONNXRUNTIME_DIR}/lib/libonnxruntime.so.1.12.0")



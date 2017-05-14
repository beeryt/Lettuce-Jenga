cmake_minimum_required(VERSION 2.8.12)
project(computer_vision)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(demo CannyDetector_Demo.cpp)
target_link_libraries(demo ${OpenCV_LIBS})

add_executable(smoothing  Smoothing.cpp)
target_link_libraries(smoothing ${OpenCV_LIBS})

add_executable(jenga jenga_detect.cpp)
target_link_libraries(jenga ${OpenCV_LIBS})
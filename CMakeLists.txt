cmake_minimum_required(VERSION 2.8)
project(image_processing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_PREFIX_PATH "/usr/local/share/OpenCV")

file(GLOB SOURCES src/*.cpp)
set(HEADER_FILES "include/" *.h)

set(OpenCV_INCLUDE_DIRS "/usr/local/include")
set(OpenCV_LIBS opencv_calib3d opencv_core opencv_features2d opencv_flann
opencv_highgui opencv_imgcodecs opencv_imgproc opencv_ml opencv_objdetect
opencv_photo opencv_shape opencv_stitching opencv_superres opencv_video
opencv_videoio opencv_videostab)

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# Add OpenCV headers location to your include paths
include_directories(${OpenCV_INCLUDE_DIRS} ${HEADER_FILES})

# Declare the executable target built from your sources
add_executable(executable ${SOURCES})

# Link your application with OpenCV libraries
target_link_libraries(executable ${OpenCV_LIBS})

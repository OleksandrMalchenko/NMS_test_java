cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_BUILD_TYPE "release")


file(GLOB srcs
        "*.cpp"
    )

add_library(yolo-jni SHARED ${srcs})
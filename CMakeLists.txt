cmake_minimum_required(VERSION 3.9)

project(sisdk VERSION 1.1.0 DESCRIPTION "Sietium SDK and Samples")

cmake_policy(SET CMP0072 OLD)
set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    "${CMAKE_MODULE_PATH}"
)

find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

add_subdirectory(sisdk)

find_package(SISDK)
if (SISDK_FOUND)
    add_subdirectory(samples)
endif(SISDK_FOUND)


cmake_minimum_required(VERSION 3.9)

project(sisdk
    VERSION 1.1.0
    DESCRIPTION "Sietium SDK and Samples"
    LANGUAGES CXX)

set(CMAKE_C_COMPILER g++)
if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11)
endif(CMAKE_COMPILER_IS_GNUCXX)

cmake_policy(SET CMP0072 OLD)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

add_subdirectory(sisdk)

find_package(Doxygen)
if(Doxygen_FOUND)
    add_subdirectory(doc)
else()
    message(STATUS "Doxygen not found, not building docs")
endif()

find_package(SISDK)
if (SISDK_FOUND)
    add_subdirectory(samples)
endif(SISDK_FOUND)


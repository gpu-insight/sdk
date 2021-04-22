cmake_minimum_required(VERSION 3.9)

project(sis
    VERSION 1.1.0
    DESCRIPTION "Sietium Semiconductor SDK and Samples"
    LANGUAGES CXX)

cmake_policy(SET CMP0072 OLD)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(sis)
add_subdirectory(samples)

find_package(Doxygen)
if(Doxygen_FOUND)
    add_subdirectory(doc)
else()
    message(STATUS "Doxygen not found, not building docs")
endif()

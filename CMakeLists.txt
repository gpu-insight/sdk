cmake_minimum_required(VERSION 3.9)

project(sisdk VERSION 1.1.0 DESCRIPTION "Sietium SDK and Samples")

cmake_policy(SET CMP0072 OLD)

find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

add_subdirectory(sisdk)
add_subdirectory(samples bin EXCLUDE_FROM_ALL)

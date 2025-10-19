# TreeVisualizer

cmake_minimum_required(VERSION 3.27)

project(TreeVisualizer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Application)
add_subdirectory(ThirdParty)
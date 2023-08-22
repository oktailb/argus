cmake_minimum_required(VERSION 3.5)

project(edgeBlending VERSION 0.1 LANGUAGES CXX)
#set(CMAKE_BUILD_TYPE release)
include("build.cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(lib)
add_subdirectory(capturer)
add_subdirectory(drawer)
add_subdirectory(debugger)

cmake_minimum_required(VERSION 3.5)

project(edgeBlendingCapturer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
        src/Capturer.cpp
        src/input.cpp      include/input.hpp
)
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME} opengl32 glu32 d3d9 d3dx9 dxgi argus)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ../lib/include)

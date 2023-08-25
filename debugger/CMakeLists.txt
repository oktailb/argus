cmake_minimum_required(VERSION 3.5)

project(argusDebugger VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
        src/main.cpp
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

if (WIN32)
    set(DEBUGGER_DEPS opengl32 glu32)
else()
    set(DEBUGGER_DEPS GL GLU X11 Xext)
endif(WIN32)

target_link_libraries(${PROJECT_NAME} PRIVATE ${DEBUGGER_DEPS} argus)
include_directories(${CMAKE_SOURCE_DIR}/include include ../capturer/include ../lib/include)


cmake_minimum_required(VERSION 3.5)

project(argus VERSION 00.01.00 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DVERSION="${CMAKE_PROJECT_VERSION}")

if(WIN32)
link_directories(${CMAKE_SOURCE_DIR}/cots/lib)
include_directories(${CMAKE_SOURCE_DIR}/cots/include)
endif(WIN32)


set(PROJECT_SOURCES
                                include/argus.h
                                include/types.h
    src/shm.cpp                 include/shm.h
    src/desktop.cpp             include/desktop.h
    src/configuration.cpp       include/configuration.h
    src/webServer.cpp           include/webServer.h
    src/webRenderer.cpp         include/webRenderer.h
    src/subprocessrunner.cpp    include/subprocessrunner.h
    src/input.cpp               include/input.hpp
    src/inputGDI.cpp
    src/inputDirectX.cpp
    src/inputXorg.cpp
)

add_library(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)
if (WIN32)
    set(ARGUS_DEPS opengl32 glu32 d3d11 d3dx11 dxgi)
else()
    set(ARGUS_DEPS GL GLU X11 Xext Xrandr)
endif(WIN32)
target_link_libraries(${PROJECT_NAME} ${ARGUS_DEPS})
include_directories(${CMAKE_SOURCE_DIR}/include include)

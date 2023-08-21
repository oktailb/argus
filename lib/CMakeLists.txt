cmake_minimum_required(VERSION 3.5)

project(argus VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(PROJECT_SOURCES
                                include/types.h
    src/shm.cpp                 include/shm.h
    src/desktop.cpp             include/desktop.h
    src/configuration.cpp       include/configuration.h
)

        add_library(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )

target_link_libraries(${PROJECT_NAME} opengl32 glu32)
include_directories(${CMAKE_SOURCE_DIR}/include include)

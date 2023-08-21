cmake_minimum_required(VERSION 3.5)

project(edgeBlendingDrawer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLEW REQUIRED)
message("GLEW location: " ${GLEW_INCLUDE_DIRS})

set(PROJECT_SOURCES
        src/main.cpp
                                include/draw.h
        src/glwidget.cpp        include/glwidget.h
        src/window.cpp          include/window.h
        src/drawQuadRec.cpp
        src/calcQuad.cpp
        src/draw.cpp
        src/drawCross.cpp
        src/drawQuad.cpp
        src/drawQuadFdf.cpp
        src/drawQuadFdfRec.cpp

)

        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )

target_link_libraries(${PROJECT_NAME} opengl32 glu32 argus GLEW::GLEW)
include_directories(${CMAKE_SOURCE_DIR}/include include ../capturer/include ../lib/include ${GLEW_INCLUDE_DIRS})

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

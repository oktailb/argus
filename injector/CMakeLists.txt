cmake_minimum_required(VERSION 3.5)

project(argusInjector VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DVERSION="${CMAKE_PROJECT_VERSION}")

set(PROJECT_SOURCES_INJECTOR
        src/inputInjector.cpp
)

set(PROJECT_SOURCES_GLHOOK
        src/openGLHook.cpp
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES_INJECTOR}
)

add_library(${PROJECT_NAME}_opengl
    ${PROJECT_SOURCES_GLHOOK}
)

if (WIN32)
    set(INJECTOR_DEPS)
    set(INJECTOR_GLHOOK_DEPS opengl32 glu32 d3d9 d3dx9 dxgi)
else()
    set(INJECTOR_GLHOOK_DEPS)
endif(WIN32)

target_link_libraries(${PROJECT_NAME} ${INJECTOR_DEPS})
target_link_libraries(${PROJECT_NAME}_opengl ${INJECTOR_GLHOOK_DEPS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ../lib/include)

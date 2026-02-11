project(argus VERSION 00.01.00 LANGUAGES CXX)

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(PIPEWIRE libpipewire-0.3)
    pkg_check_modules(GIO gio-2.0)
    if (PIPEWIRE_FOUND AND GIO_FOUND)
        # add_definitions(-DENABLE_WAYLAND)
    elseif (PIPEWIRE_FOUND)
        message(WARNING "PipeWire found but GIO (GDBus) missing. Wayland support disabled.")
    endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DVERSION="${CMAKE_PROJECT_VERSION}")

if(WIN32)
link_directories(${CMAKE_SOURCE_DIR}/cots/lib)
include_directories(${CMAKE_SOURCE_DIR}/cots/include)
endif(WIN32)


set(PROJECT_SOURCES
    src/argus.cpp               include/argus.h
                                include/types.h
    src/shm.cpp                 include/shm.h
    src/desktop.cpp             include/desktop.h
    src/configuration.cpp       include/configuration.h
    src/argusConfig.cpp         include/argusConfig.h
    src/webServer.cpp           include/webServer.h
    src/webRenderer.cpp         include/webRenderer.h
    src/subprocessrunner.cpp    include/subprocessrunner.h
    src/input.cpp               include/input.hpp
    src/inputGDI.cpp
    src/inputDirectX.cpp
    src/inputXorg.cpp
    src/inputWayland.cpp        include/inputWayland.hpp
)

add_library(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)
if (WIN32)
    set(ARGUS_DEPS opengl32 glu32 d3d11 d3dx11 dxgi)
else()
    set(ARGUS_DEPS GL GLU X11 Xext Xrandr)
endif(WIN32)

if (PIPEWIRE_FOUND)
    list(APPEND ARGUS_DEPS ${PIPEWIRE_LIBRARIES} ${GIO_LIBRARIES})
    include_directories(${PIPEWIRE_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
    if (GIO_FOUND)
        target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_WAYLAND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${PIPEWIRE_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
    endif()
endif()

target_link_libraries(${PROJECT_NAME} ${ARGUS_DEPS})
include_directories(${CMAKE_SOURCE_DIR}/include include)

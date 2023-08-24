cmake_minimum_required(VERSION 3.5)

project(edgeBlendingDrawer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GLEW REQUIRED)
message("GLEW location: " ${GLEW_INCLUDE_DIRS})

if(WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/cots/lib)
    include_directories(${CMAKE_SOURCE_DIR}/cots/include)
endif(WIN32)

# Creates C resources file from files in given directory
function(create_resources dir output)
    # Create empty output file
    file(WRITE ${output} "")
    # Collect input files
    file(GLOB bins ${dir}/*)
    # Iterate through input files
    foreach(bin ${bins})
        # Get short filename
        string(REGEX MATCH "([^/]+)$" filename ${bin})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        # Read hex data from file
        file(READ ${bin} filedata HEX)
        # Convert hex data for C compatibility
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        # Append data to output file
        file(APPEND ${output} "const unsigned char ${filename}[] = {${filedata}};\nconst unsigned ${filename}_size = sizeof(${filename});\n")
    endforeach()
endfunction()

create_resources(${CMAKE_CURRENT_LIST_DIR}/res ${CMAKE_CURRENT_LIST_DIR}/include/resources.h)

# Display resources in IDE
add_custom_target(resources SOURCES
      ${resources_files}
)

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
                                include/resources.h
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${resources_files_objects}
)

if (WIN32)
    set(DRAWER_DEPS opengl32 glu32)
else()
    set(DRAWER_DEPS GL GLU X11 Xext)
endif(WIN32)

target_link_libraries(${PROJECT_NAME} ${DRAWER_DEPS} argus GLEW::GLEW microhttpd)
include_directories(${CMAKE_SOURCE_DIR}/include include ../capturer/include ../lib/include ${GLEW_INCLUDE_DIRS})

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})


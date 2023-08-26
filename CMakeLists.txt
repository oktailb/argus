cmake_minimum_required(VERSION 3.5)

project(argus   VERSION 0.1
                LANGUAGES CXX
                DESCRIPTION "Software performing realtime geometry and blending utility for large media applications"
                HOMEPAGE_URL "https://github.com/oktailb/argus"
            )
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

install(TARGETS ${PROJECT_NAME}Capturer
                ${PROJECT_NAME}Drawer

    BUNDLE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/config.ini

    DESTINATION .
)

if (WIN32)
#    install(FILES ${CMAKE_CXX_COMPILER}/bin/*.dll

#        DESTINATION .
#    )
endif()

set(CPACK_RESOURCE_FILE_LICENSE  "${CMAKE_CURRENT_LIST_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "vincent.lecoq@gmail.com")
set(CPACK_PACKAGE_VENDOR "Oktail Mouling corp")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "TGZ;RPM;DEB")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_GENERATOR "PACKAGEMAKER;DRAGANDROP;BUNDLE;OSXX11")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

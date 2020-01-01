cmake_minimum_required(VERSION 3.14)

set(PROJECT_LIB dehancer_client_cpp)
project(${PROJECT_LIB} VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 17)

#
# Package manager
#
include(ExternalProject)

#
# Packages
#
find_package (PkgConfig QUIET)
find_package (Threads REQUIRED)
find_package (CURL REQUIRED)
find_package (ZLIB REQUIRED)

#
# Global setttings
#
set(VERSION_FILE "version.txt")
set(CMAKE_CXX_STANDARD 17)

# googletes, etc
set (EXTERNAL_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/external)
set (EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set (EXTERNAL_INCLUDE_DIR ${EXTERNAL_INSTALL_LOCATION}/include)
set (EXIVEDK_DB_DIR ${CMAKE_SOURCE_DIR}/external/exiv2)
set (COMMON_DEPENDECIES)
set(COMMON_LIBRARIES  ${ZLIB_LIBRARIES}; ${CURL_LIBRARIES};)

#
# Generators
#
file (WRITE "${VERSION_FILE}" "${PROJECT_NAME} ${PROJECT_VERSION}")
configure_file(include/dehancer/client/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h)

#
# Rx
#

ExternalProject_Add(rxcpp
        GIT_REPOSITORY https://github.com/ReactiveX/RxCpp
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        BUILD_ALWAYS NO
        BUILD_COMMAND echo "skip..."
        INSTALL_COMMAND make install
        )
set(COMMON_DEPENDECIES ${COMMON_DEPENDECIES};rxcpp)

#
# Google Test
#
find_package(GTest)
if (NOT GTEST_FOUND)
    ExternalProject_Add(googletest
            GIT_REPOSITORY https://github.com/google/googletest
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
            )
    set(COMMON_DEPENDECIES googletest)
endif ()

ExternalProject_Add(ed25519cpplib
        GIT_REPOSITORY https://github.com/dnevera/ed25519cpp
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
        -DBUILD_TESTING=OFF
        INSTALL_COMMAND make install
        PREFIX=${CMAKE_BINARY_DIR}/ed25519cpplib
        )
set(COMMON_DEPENDECIES ${COMMON_DEPENDECIES};ed25519cpplib)

ExternalProject_Add(base64cpplib
        GIT_REPOSITORY https://github.com/dnevera/base64cpp
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
        -DBUILD_TESTING=OFF
        INSTALL_COMMAND make install
        PREFIX=${CMAKE_BINARY_DIR}/base64cpplib
        )
set(COMMON_DEPENDECIES ${COMMON_DEPENDECIES};base64cpplib)

#
# Common
#
pkg_check_modules(dehancer_common_cpp dehancer-common-cpp)
if (dehancer_common_cpp_FOUND)

    message(STATUS "dehancer_common_cpp: ${dehancer_common_cpp_LIBRARIES}, ${dehancer_common_cpp_LIBRARY_DIRS} ${dehancer_common_cpp_INCLUDE_DIRS}")
    include_directories(
            ${dehancer_common_cpp_INCLUDE_DIRS}
    )
    link_directories(
            ${dehancer_common_cpp_LIBRARY_DIRS}
    )
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES};${dehancer_common_cpp_LIBRARIES})

else()
    ExternalProject_Add(dehancer_common_cpplib
            GIT_REPOSITORY https://github.com/dehancer/dehancer-common-cpp
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
            -DCMAKE_OSX_DEPLOYMENT_TARGET=${DCMAKE_OSX_DEPLOYMENT_TARGET}
            )
    set(COMMON_DEPENDECIES ${COMMON_DEPENDECIES};dehancer_common_cpplib)
    set(COMMON_LIBRARIES  ${COMMON_LIBRARIES};dehancer_common_cpp)
endif()

#
# Details
#
add_definitions( -DCURL_STATICLIB )
include_directories(
        ${CURL_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/external/include
        ${CMAKE_SOURCE_DIR}/include
)

link_directories(
        ${CMAKE_BINARY_DIR}/external/lib
)

add_subdirectory(lib)

if(BUILD_TESTING)
    add_subdirectory(tests)
    enable_testing ()
endif()

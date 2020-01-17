cmake_minimum_required(VERSION 3.15)

project(BLIB C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(STATIC_LINK "On to link statically" On)
option(BUILD_TESTS "On to build unit tests" On)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_INSTALL_PREFIX .)

if (STATIC_LINK)
    add_library(BLIB STATIC)
else()
    add_library(BLIB SHARED)
endif()

if (STATIC_LINK)
    set(BUILD_SHARED_LIBS Off)
    set(SFML_USE_STATIC_STD_LIBS On)
    add_definitions(-DSFML_STATIC)
endif()

include_directories(lib/SFML/include)
add_subdirectory(lib/SFML)

add_subdirectory(include/BLIB)
add_subdirectory(src)
include_directories(include)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(lib/gtest)
    add_subdirectory(tests)
endif()
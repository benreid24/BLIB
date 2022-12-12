cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(BLIB C CXX)

option(BUILD_TESTS "On to build unit tests" On)
option(BUILD_EXAMPLES "On to build examples" On)
option(ON_CI "True when on CI, false when local" Off)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (ON_CI)
    add_compile_definitions(ON_CI)
endif()

set(CMAKE_INSTALL_PREFIX .)

add_library(BLIB STATIC)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(BLIB PUBLIC BLIB_DEBUG)
else()
    target_compile_definitions(BLIB PUBLIC BLIB_RELEASE)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(blib_helpers)
configure_target(BLIB)

set(BUILD_SHARED_LIBS Off)
set(SFML_USE_STATIC_STD_LIBS On)
add_compile_definitions(SFML_STATIC)

# shouldn't need this but...
target_compile_definitions(BLIB PUBLIC SFML_STATIC)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

include_directories(lib/SFML/include)
add_subdirectory(lib/SFML)
set_target_properties(sfml-audio PROPERTIES CXX_STANDARD 98 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)

add_subdirectory(include/BLIB)
add_subdirectory(src)
include_directories(include)

target_link_libraries(BLIB PRIVATE
    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(lib/gtest)
    add_subdirectory(tests)
endif()

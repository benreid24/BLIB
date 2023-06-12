cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(BLIB C CXX)

option(BUILD_TESTS "On to build unit tests" On)
option(BUILD_EXAMPLES "On to build examples" On)
option(BLIB_HEADLESS_FOR_CI_TESTING "True when on CI, false when local" Off)

option(BLIB_ECS_USE_WIDE_MASK "True to use 128 bit component mask, false for 64 bit component mask" Off)
option(COMPILE_SHADERS "True to build built-in shaders, off to skip" On)
set(MAX_DESCRIPTOR_BINDINGS "8" CACHE STRING "Maximum number of bindings in a descriptor set")
set(RUN_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Working directory of the final built executables")
set(SHADER_PATH "Resources/Shaders" CACHE PATH "Path where compiled shaders should go")
set(SCENE_DEFAULT_CAMERA "2d" CACHE STRING "Either '2d' or '3d'. Determines which type of camera is created for scenes by default")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_INSTALL_PREFIX .)

add_library(BLIB STATIC)

# Make vulkan dir in build dir to fix vma include
set(BLIB_TEMP_INCLUDE_DIR ${CMAKE_BINARY_DIR}/blib_temp_includes)
file(MAKE_DIRECTORY ${BLIB_TEMP_INCLUDE_DIR})

set(BLIB_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(
    CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/SFML/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/SFML/cmake/modules"
)
include(configure_blib_target)
configure_blib_target(BLIB)

set(BUILD_SHARED_LIBS Off)
add_compile_definitions(SFML_STATIC)
set(SFML_USE_STATIC_STD_LIBS On)
add_subdirectory(lib/SFML)
set_target_properties(sfml-audio PROPERTIES CXX_STANDARD 98 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)

add_subdirectory(lib/glad)

add_subdirectory(include/BLIB)
add_subdirectory(src)

target_link_libraries(BLIB PRIVATE
    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
    Freetype
)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(lib/gtest)
    add_subdirectory(tests)
endif()

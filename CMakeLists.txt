cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif()

project(BLIB C CXX)

set(BLIB_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(BLIB_PATH ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

# Determine glslc path
if(APPLE)
    set(DEFAULT_GLSLC_PATH "${BLIB_PATH}/tools/glslc/Macos/glslc")
elseif(WIN32)
    set(DEFAULT_GLSLC_PATH "${BLIB_PATH}/tools/glslc/Windows/glslc.exe")
else()
    # TODO - Pack Linux binary
    set(DEFAULT_GLSLC_PATH "glslc")
endif()

option(BUILD_TESTS "On to build unit tests" On)
option(BUILD_EXAMPLES "On to build examples" On)

option(BLIB_ECS_USE_WIDE_MASK "True to use 128 bit component mask, false for 64 bit component mask" Off)
set(RUN_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Working directory of the final built executables")
set(SHADER_PATH "Resources/Shaders" CACHE PATH "Path where compiled shaders should go")
set(GLSLC_PATH "${DEFAULT_GLSLC_PATH}" CACHE STRING "Path to glslc to use to compile shaders")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_INSTALL_PREFIX .)

add_library(BLIB STATIC)

# Make vulkan dir in build dir to fix vma include
set(BLIB_TEMP_INCLUDE_DIR ${CMAKE_BINARY_DIR}/blib_temp_includes)
file(MAKE_DIRECTORY ${BLIB_TEMP_INCLUDE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(set_module_path)
set_module_path()

include(configure_blib_target)
configure_blib_target(BLIB)

set(BUILD_SHARED_LIBS Off)
add_compile_definitions(SFML_STATIC)
set(SFML_USE_STATIC_STD_LIBS On)
add_subdirectory(lib/SFML)

add_subdirectory(lib/volk)
target_include_directories(volk SYSTEM PUBLIC ${BLIB_PATH}/lib/Vulkan-Headers/include)

if(APPLE)
    set(GLFW_LIBRARY_TYPE "STATIC")
    add_subdirectory(lib/glfw)
endif()

add_subdirectory(include/BLIB)
add_subdirectory(src)

include(link_blib_target)
link_blib_target(BLIB)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(lib/gtest)
    add_subdirectory(tests)
endif()

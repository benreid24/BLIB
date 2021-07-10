cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(BLIB C CXX)

option(STATIC_LINK "On to link statically" On)
option(BUILD_TESTS "On to build unit tests" On)
option(BUILD_EXAMPLES "On to build examples" On)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_INSTALL_PREFIX .)

if (STATIC_LINK)
    add_library(BLIB STATIC)
else()
    add_library(BLIB SHARED)
endif()

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(BLIB PUBLIC /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(BLIB PUBLIC -Wall -Wextra -pedantic)
endif()

if (STATIC_LINK)
    set(BUILD_SHARED_LIBS Off)
    set(SFML_USE_STATIC_STD_LIBS On)
    add_definitions(-DSFML_STATIC)
endif()

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

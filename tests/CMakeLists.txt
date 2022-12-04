add_executable(BLIB.t)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(BLIB.t PUBLIC /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(BLIB.t PUBLIC -Wall -Wextra -pedantic)
endif()

set(BUILD_SHARED_LIBS Off)
set(SFML_USE_STATIC_STD_LIBS On)
target_compile_definitions(BLIB.t PUBLIC SFML_STATIC BLIB_TEST_TARGET)
set_target_properties(sfml-audio PROPERTIES CXX_STANDARD 98 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if (CMAKE_HOST_WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBRARIES OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

target_link_libraries(BLIB.t PRIVATE
    BLIB

    sfml-graphics
    sfml-window
    sfml-network
    sfml-system
    
    gtest
    gmock
    gtest_main
)

add_subdirectory(AI)
add_subdirectory(Containers)
add_subdirectory(ECS)
add_subdirectory(Engine)
add_subdirectory(Events)
add_subdirectory(Math)
add_subdirectory(Parser)
add_subdirectory(Resources)
add_subdirectory(Scripts)
add_subdirectory(Serialization)
add_subdirectory(Util)

target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googletest/include)
target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googlemock/include)

add_test(NAME UnitTests COMMAND BLIB.T)

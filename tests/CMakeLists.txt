add_executable(BLIB.t)

include(configure_blib_target)
configure_blib_target(BLIB.t)
if (MSVC)
    target_compile_options(BLIB.t PUBLIC /wd4389)
endif()
target_compile_definitions(BLIB.t PUBLIC BLIB_TEST_TARGET)

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

set_target_properties(BLIB.t
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME UnitTests COMMAND BLIB.t)

add_executable(BLIB.t)

if (MSVC)
    # warning level 4 and all warnings as errors
    target_compile_options(BLIB.t PUBLIC /W4 /WX)
else()
    # lots of warnings and all warnings as errors
    target_compile_options(BLIB.t PUBLIC -Wall -Wextra -pedantic)
endif()

target_link_libraries(BLIB.t PRIVATE
    BLIB

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
    
    gtest
    gmock
    gtest_main
)

add_subdirectory(AI)
add_subdirectory(Containers)
add_subdirectory(Engine)
add_subdirectory(Entities)
add_subdirectory(Events)
add_subdirectory(Files)
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

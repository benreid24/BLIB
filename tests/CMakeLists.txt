add_executable(BLIB.t
    ResourceManager.t.cpp
)

target_link_libraries(BLIB.t PRIVATE
    BLIB

    gtest
    gtest_main
)

add_subdirectory(Files)
add_subdirectory(Parser)
add_subdirectory(Scripts)

target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googletest/include)
target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googlemock/include)

add_test(NAME UnitTests COMMAND BLIB.T)
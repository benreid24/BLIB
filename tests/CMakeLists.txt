add_executable(BLIB.t
    ResourceManager.t.cpp
)

target_link_libraries(BLIB.t PRIVATE 
    gtest
    gtest_main
)


target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googletest/include)
target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googlemock/include)

add_test(COMMAND BLIB.T)
add_executable(BLIB.t
    BinaryFile.t.cpp
    FileUtil.t.cpp
    JSON.t.cpp
    ResourceManager.t.cpp
    Schema.t.cpp
)

target_link_libraries(BLIB.t PRIVATE
    BLIB

    gtest
    gtest_main
)


target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googletest/include)
target_include_directories(BLIB.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googlemock/include)

add_test(NAME UnitTests COMMAND BLIB.T)
add_executable(BENG.t
    ResourceManager.t.cpp
)

target_link_libraries(BENG.t PRIVATE
    BENG

    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
    
    gtest
    gmock
    gtest_main
)

add_subdirectory(Files)
add_subdirectory(Parser)
add_subdirectory(Scripts)
add_subdirectory(Util)

target_include_directories(BENG.t PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(BENG.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googletest/include)
target_include_directories(BENG.t PRIVATE ${PROJECT_SOURCE_DIR}/lib/gtest/googlemock/include)

add_test(NAME UnitTests COMMAND BENG.T)
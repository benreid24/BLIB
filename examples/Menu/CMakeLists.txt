add_executable(menu main.cpp)

target_link_libraries(menu
    BLIB
    sfml-graphics
    sfml-window
)

set_target_properties(menu
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

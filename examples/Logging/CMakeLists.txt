add_executable(logging main.cpp)

target_link_libraries(logging
    BLIB
    sfml-graphics
    sfml-window
)

set_target_properties(logging
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
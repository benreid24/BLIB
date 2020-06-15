add_executable(shapes main.cpp)

target_link_libraries(shapes
    BLIB
    sfml-graphics
    sfml-window
)

set_target_properties(shapes
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
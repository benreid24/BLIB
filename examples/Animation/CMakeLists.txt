add_executable(animation main.cpp)

target_link_libraries(animation
    BLIB
    sfml-graphics
    sfml-window
)

set_target_properties(animation
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
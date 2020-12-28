add_executable(gui main.cpp)

target_link_libraries(gui
    BENG
    sfml-graphics
    sfml-window
)

set_target_properties(gui
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
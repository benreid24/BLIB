add_executable(resources main.cpp)

target_link_libraries(resources
    BLIB
    sfml-graphics
    sfml-window
)

set_target_properties(resources
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
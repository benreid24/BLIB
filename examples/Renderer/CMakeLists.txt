add_executable(renderer main.cpp)

include(blib_helpers)
configure_target(renderer)

target_link_libraries(renderer
    BLIB
    sfml-graphics
    sfml-window
)

set_target_properties(renderer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

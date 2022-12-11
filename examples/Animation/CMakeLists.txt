add_executable(animation main.cpp)

target_link_libraries(animation
    BLIB
    sfml-graphics
    sfml-audio
    sfml-window
)

set_target_properties(animation
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

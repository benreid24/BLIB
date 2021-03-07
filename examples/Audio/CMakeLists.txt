add_executable(audio main.cpp)

target_link_libraries(audio
    BLIB
    sfml-graphics
    sfml-audio
    sfml-window
)

set_target_properties(audio
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

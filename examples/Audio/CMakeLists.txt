add_executable(audio main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(audio)
link_blib_target(audio)

set_target_properties(audio
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

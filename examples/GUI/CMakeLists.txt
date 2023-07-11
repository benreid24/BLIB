add_executable(gui main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(gui)
link_blib_target(gui)

set_target_properties(gui
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

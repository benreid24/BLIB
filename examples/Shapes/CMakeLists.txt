add_executable(shapes main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(shapes)
link_blib_target(shapes)

set_target_properties(shapes
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

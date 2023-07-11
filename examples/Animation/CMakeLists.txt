add_executable(animation main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(animation)
link_blib_target(animation)

set_target_properties(animation
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

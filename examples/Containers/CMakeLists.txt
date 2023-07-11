add_executable(containers main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(containers)
link_blib_target(containers)

set_target_properties(containers
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

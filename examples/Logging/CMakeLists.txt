add_executable(logging main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(logging)
link_blib_target(logging)

set_target_properties(logging
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(menu main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(menu)
link_blib_target(menu)

set_target_properties(menu
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

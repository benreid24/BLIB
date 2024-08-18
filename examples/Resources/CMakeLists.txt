add_executable(resources
    main.cpp
    Character.cpp
    Map.cpp
)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(resources)
link_blib_target(resources)

set_target_properties(resources
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

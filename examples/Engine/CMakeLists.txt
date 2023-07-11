add_executable(engine 
    InputListener.cpp
    main.cpp
    MainState.cpp
    RebindState.cpp
)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(engine)
link_blib_target(engine)

set_target_properties(engine
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(engine 
    InputListener.cpp
    main.cpp
    MainState.cpp
    RebindState.cpp
)

target_link_libraries(engine
    BLIB
    sfml-graphics
    sfml-window
    sfml-network
    sfml-audio
    sfml-system
)

set_target_properties(engine
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

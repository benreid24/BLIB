add_executable(json_serial
    main.cpp
)

target_link_libraries(json_serial
    BLIB 
)

set_target_properties(json_serial
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
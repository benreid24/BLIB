add_executable(files
    Data.cpp
    main.cpp
)

target_link_libraries(files
    BENG 
)

set_target_properties(files
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
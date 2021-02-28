add_executable(files_basic
    Data.cpp
    main.cpp
)

target_link_libraries(files_basic
    BLIB 
)

set_target_properties(files_basic
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
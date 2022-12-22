add_executable(bvulkan
    main.cpp
    Renderer.hpp
    Renderer.cpp
)

include(blib_helpers)
configure_target(bvulkan)

add_subdirectory(utils)

target_link_libraries(bvulkan
    BLIB
    sfml-graphics
    sfml-window
)

set_target_properties(bvulkan
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

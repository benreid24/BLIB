add_executable(renderer main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(renderer)
link_blib_target(renderer)

set_target_properties(renderer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Copy compiled shaders. This step is not needed for top-level targets that use BLIB as a module
add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy_shaders.cmake
    DEPENDS blib_builtin_shaders
)
add_dependencies(renderer copy_shaders)

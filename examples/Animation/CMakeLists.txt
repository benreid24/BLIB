add_executable(animation main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(animation)
link_blib_target(animation)

set_target_properties(animation
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Copy compiled shaders. This step is not needed for top-level targets that set SHADER_PATH
if(TARGET blib_builtin_shaders)
    add_custom_target(copy_animation_shaders
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy_shaders.cmake
        DEPENDS blib_builtin_shaders
    )
    add_dependencies(animation copy_animation_shaders)
else()
    message(WARNING "Animation example requires the built-in shaders to be compiled. Specify glslc path in GLSLC_PATH")
endif()

add_executable(graphics3d main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(graphics3d)
link_blib_target(graphics3d)

set_target_properties(graphics3d
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Copy compiled shaders. This step is not needed for top-level targets that set SHADER_PATH
if(TARGET blib_builtin_shaders)
    add_custom_target(copy_graphics3d_shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${SHADER_PATH}" "${CMAKE_CURRENT_LIST_DIR}/${SHADER_PATH}"
        DEPENDS blib_builtin_shaders
        COMMENT "Copying shaders to Graphics3D example"
    )
    add_dependencies(graphics3d copy_graphics3d_shaders)
else()
    message(WARNING "Graphics3D example requires the built-in shaders to be compiled. Specify glslc path in GLSLC_PATH")
endif()

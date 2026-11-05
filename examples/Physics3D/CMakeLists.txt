include(create_blib_target)
create_blib_target(
    TARGET physics3d
    OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_FILE "physics3d"
    SOURCES main.cpp
)

# Copy compiled shaders. This step is not needed for top-level targets that set SHADER_PATH
if(TARGET blib_builtin_shaders)
    add_custom_target(copy_physics3d_shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${SHADER_PATH}" "${CMAKE_CURRENT_LIST_DIR}/${SHADER_PATH}"
        DEPENDS blib_builtin_shaders
        COMMENT "Copying shaders to Physics3D example"
    )
    add_dependencies(physics3d copy_physics3d_shaders)
else()
    message(WARNING "Physics3D example requires the built-in shaders to be compiled. Specify glslc path in GLSLC_PATH")
endif()

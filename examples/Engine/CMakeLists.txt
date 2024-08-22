include(create_blib_target)
create_blib_target(
    TARGET engine
    OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_FILE "engine"
    SOURCES main.cpp InputListener.cpp MainState.cpp RebindState.cpp
)

# Copy compiled shaders. This step is not needed for top-level targets that set SHADER_PATH
if(TARGET blib_builtin_shaders)
    add_custom_target(copy_engine_shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${SHADER_PATH}" "${CMAKE_CURRENT_LIST_DIR}/${SHADER_PATH}"
        DEPENDS blib_builtin_shaders
        COMMENT "Copying shaders to Engine example"
    )
    add_dependencies(engine copy_engine_shaders)
else()
    message(WARNING "Engine example requires the built-in shaders to be compiled. Specify glslc path in GLSLC_PATH")
endif()

add_executable(audio main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(audio)
link_blib_target(audio)

set_target_properties(audio
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Copy compiled shaders. This step is not needed for top-level targets that set SHADER_PATH
if(TARGET blib_builtin_shaders)
    add_custom_target(copy_audio_shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${SHADER_PATH}" "${CMAKE_CURRENT_LIST_DIR}/${SHADER_PATH}"
        DEPENDS blib_builtin_shaders
        COMMENT "Copying shaders to Audio example"
    )
    add_dependencies(audio copy_audio_shaders)
else()
    message(WARNING "Audio example requires the built-in shaders to be compiled. Specify glslc path in GLSLC_PATH")
endif()

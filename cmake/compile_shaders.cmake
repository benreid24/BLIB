function(compile_shaders)
    include(find_glslc)
    find_glslc(glslc_found glslc_binary)
    if(NOT ${glslc_found})
        message(WARNING "Skipping shader compilation")
        return()
    endif()

    # Parse arguments
    cmake_parse_arguments(
        PARSE_ARGV
        0
        ARG                    # Prefix
        ""                     # No bool args
        "TARGET;SHADER_MODULE" # Single value args
        "SHADERS"              # Multi-valued args
    )

    # Validate args
    if(NOT DEFINED ARG_TARGET)
        message(FATAL_ERROR "compile_shaders requires a TARGET")
    endif()
    if(NOT DEFINED ARG_SHADER_MODULE)
        message(FATAL_ERROR "compile_shaders requires a SHADER_MODULE name")
    endif()
    if(NOT DEFINED ARG_SHADERS)
        message(FATAL_ERROR "compile_shaders requires SHADERS list of sources")
    endif()

    # Provide macro for shader path and compute full shader path for build
    target_compile_definitions(${ARG_TARGET} PUBLIC SHADER_PATH=${SHADER_PATH})
    set(shader_dir "${RUN_PATH}/${SHADER_PATH}")
    file(MAKE_DIRECTORY ${shader_dir})

    # Configure build commands for each shader
    set(compiled_files "")
    foreach(shader_file ${ARG_SHADERS})
        set(compiled_file "${shader_dir}/${shader_file}.spv")
        list(APPEND compiled_files "${compiled_file}")
        add_custom_command(
            OUTPUT ${compiled_file}
            COMMENT "Compiling shader '${shader_file}'"
            COMMAND ${glslc_binary} ${shader_file} -o ${compiled_file}
            DEPENDS ${shader_file}
            MAIN_DEPENDENCY ${shader_file}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endforeach()

    # Create shader module target
    add_custom_target(${ARG_SHADER_MODULE} DEPENDS ${compiled_files})
    add_dependencies(${ARG_TARGET} ${ARG_SHADER_MODULE})
endfunction()

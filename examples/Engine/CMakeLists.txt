add_executable(engine 
    InputListener.cpp
    main.cpp
    MainState.cpp
    RebindState.cpp
)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(engine)
link_blib_target(engine)

set_target_properties(engine
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
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

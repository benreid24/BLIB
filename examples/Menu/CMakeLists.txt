add_executable(menu main.cpp)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(menu)
link_blib_target(menu)

set_target_properties(menu
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Copy compiled shaders. This step is not needed for top-level targets that set SHADER_PATH
if(TARGET blib_builtin_shaders)
    add_custom_target(copy_menu_shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${SHADER_PATH}" "${CMAKE_CURRENT_LIST_DIR}/${SHADER_PATH}"
        DEPENDS blib_builtin_shaders
        COMMENT "Copying shaders to Menu example"
    )
    add_dependencies(menu copy_menu_shaders)
else()
    message(WARNING "Menu example requires the built-in shaders to be compiled. Specify glslc path in GLSLC_PATH")
endif()

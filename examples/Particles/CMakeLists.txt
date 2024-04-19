set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(particles main.cpp)

add_subdirectory(Shaders)

include(configure_blib_target)
include(link_blib_target)

configure_blib_target(particles)
link_blib_target(particles)

set_target_properties(particles
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Copy compiled shaders. This step is not needed for top-level targets that set SHADER_PATH
if(TARGET blib_builtin_shaders)
    add_custom_target(copy_particles_shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${SHADER_PATH}" "${CMAKE_CURRENT_LIST_DIR}/${SHADER_PATH}"
        DEPENDS blib_builtin_shaders particle_shaders
        COMMENT "Copying shaders to Particles example"
    )
    add_dependencies(particles copy_particles_shaders)
else()
    message(WARNING "Particles example requires the built-in shaders to be compiled. Specify glslc path in GLSLC_PATH")
endif()

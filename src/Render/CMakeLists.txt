target_sources(BLIB PRIVATE
    Cameras.cpp
    Renderer.cpp
)

add_subdirectory(Cameras)
add_subdirectory(Primitives)
add_subdirectory(Renderables)
add_subdirectory(Renderer)
add_subdirectory(Resources)
add_subdirectory(Uniforms)
add_subdirectory(Vulkan)

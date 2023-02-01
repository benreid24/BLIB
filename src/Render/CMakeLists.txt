target_sources(BLIB PRIVATE
    Observer.cpp
    Renderer.cpp
)

add_subdirectory(Cameras)
add_subdirectory(Overlays)
add_subdirectory(Primitives)
add_subdirectory(Renderables)
add_subdirectory(Renderer)
add_subdirectory(Resources)
add_subdirectory(Uniforms)
add_subdirectory(Vulkan)

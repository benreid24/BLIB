target_sources(BLIB PRIVATE
    Observer.cpp
    Renderer.cpp
)

add_subdirectory(Cameras)
add_subdirectory(Descriptors)
add_subdirectory(Overlays)
add_subdirectory(Primitives)
add_subdirectory(Renderables)
add_subdirectory(Resources)
add_subdirectory(Scenes)
add_subdirectory(Shaders)
add_subdirectory(Systems)
add_subdirectory(Transfers)
add_subdirectory(Uniforms)
add_subdirectory(Vulkan)

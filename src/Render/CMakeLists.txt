target_sources(BLIB PRIVATE
    GraphicsSettings.cpp
    Observer.cpp
    Renderer.cpp
    RenderTarget.cpp
    Settings.cpp
    WindowSettings.cpp
)

add_subdirectory(Buffers)
add_subdirectory(Components)
add_subdirectory(Descriptors)
add_subdirectory(Graph)
add_subdirectory(Lighting)
add_subdirectory(Overlays)
add_subdirectory(Materials)
add_subdirectory(Primitives)
add_subdirectory(Resources)
add_subdirectory(Scenes)
add_subdirectory(ShaderResources)
add_subdirectory(Shaders)
add_subdirectory(Transfers)
add_subdirectory(Vulkan)

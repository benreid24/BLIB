target_sources(BLIB PUBLIC
    Color.hpp
    Observer.hpp
    Renderer.hpp
    RenderPhase.hpp
    RenderTarget.hpp
    Settings.hpp
    UpdateSpeed.hpp
    Window.hpp
)

add_subdirectory(Buffers)
add_subdirectory(Components)
add_subdirectory(Config)
add_subdirectory(Descriptors)
add_subdirectory(Events)
add_subdirectory(Graph)
add_subdirectory(Lighting)
add_subdirectory(Materials)
add_subdirectory(Primitives)
add_subdirectory(Resources)
add_subdirectory(Scenes)
add_subdirectory(ShaderResources)
add_subdirectory(Transfers)
add_subdirectory(Vulkan)

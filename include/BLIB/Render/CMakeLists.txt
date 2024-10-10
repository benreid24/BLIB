target_sources(BLIB PUBLIC
    Color.hpp
    Config.hpp
    Observer.hpp
    Renderer.hpp
    RenderTarget.hpp
    UpdateSpeed.hpp
)

add_subdirectory(Buffers)
add_subdirectory(Components)
add_subdirectory(Descriptors)
add_subdirectory(Events)
add_subdirectory(Graph)
add_subdirectory(Lighting)
add_subdirectory(Primitives)
add_subdirectory(Resources)
add_subdirectory(Scenes)
add_subdirectory(Transfers)
add_subdirectory(Vulkan)

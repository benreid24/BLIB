target_sources(BLIB PUBLIC
    Config.hpp
    Observer.hpp
    Renderer.hpp
    UpdateSpeed.hpp
)

add_subdirectory(Buffers)
add_subdirectory(Cameras)
add_subdirectory(Components)
add_subdirectory(Descriptors)
add_subdirectory(Drawables)
add_subdirectory(Events)
add_subdirectory(Primitives)
add_subdirectory(Resources)
add_subdirectory(Scenes)
add_subdirectory(Systems)
add_subdirectory(Transfers)
add_subdirectory(Vulkan)

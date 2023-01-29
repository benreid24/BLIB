target_sources(BLIB PUBLIC
    Config.hpp
    Observer.hpp
    Renderer.hpp
)

add_subdirectory(Cameras)
add_subdirectory(Primitives)
add_subdirectory(Renderables)
add_subdirectory(Renderer)
add_subdirectory(Resources)
add_subdirectory(Uniforms)
add_subdirectory(Util)
add_subdirectory(Vulkan)

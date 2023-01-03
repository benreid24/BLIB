target_sources(BLIB PUBLIC
    Cameras.hpp
    Config.hpp
    Ref.hpp
    Renderer.hpp
)

add_subdirectory(Cameras)
add_subdirectory(Components)
add_subdirectory(Renderables)
add_subdirectory(Renderer)
add_subdirectory(Resources)
add_subdirectory(Uniforms)
add_subdirectory(Vulkan)

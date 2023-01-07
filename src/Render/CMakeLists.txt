target_sources(BLIB PRIVATE
    Cameras.cpp
    Renderer.cpp
)

add_subdirectory(Cameras)
add_subdirectory(Components)
add_subdirectory(Renderables)
add_subdirectory(Renderer)
add_subdirectory(Vulkan)

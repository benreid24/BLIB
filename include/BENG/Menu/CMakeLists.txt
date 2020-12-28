target_sources(BENG PUBLIC
    Event.hpp
    Item.hpp
    Menu.hpp
    Renderer.hpp
    RenderItem.hpp
    Selector.hpp
)

add_subdirectory(EventGenerators)
add_subdirectory(Renderers)
add_subdirectory(RenderItems)
add_subdirectory(Selectors)

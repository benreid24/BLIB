target_sources(BLIB PUBLIC
    Event.hpp
    Item.hpp
    Menu.hpp
    Renderer.hpp
    RenderItem.hpp
)

add_subdirectory(Renderers)
add_subdirectory(RenderItems)
add_subdirectory(Selectors)

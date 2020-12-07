target_sources(BLIB PRIVATE
    Event.cpp
    Item.cpp
    Menu.cpp
)

add_subdirectory(Renderers)
add_subdirectory(RenderItems)
add_subdirectory(Selectors)

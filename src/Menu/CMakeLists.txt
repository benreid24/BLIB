target_sources(BENG PRIVATE
    Event.cpp
    Item.cpp
    Menu.cpp
)

add_subdirectory(EventGenerators)
add_subdirectory(Renderers)
add_subdirectory(RenderItems)
add_subdirectory(Selectors)

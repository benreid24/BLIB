target_sources(BLIB PUBLIC
    Event.hpp
    Item.hpp
    Menu.hpp
    Selector.hpp
)

add_subdirectory(EventGenerators)
add_subdirectory(Items)
add_subdirectory(Selectors)

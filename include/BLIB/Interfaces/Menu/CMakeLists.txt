target_sources(BLIB PUBLIC
    Event.hpp
    Item.hpp
    Menu.hpp
    Selector.hpp
)

add_subdirectory(Drivers)
add_subdirectory(Items)
add_subdirectory(Selectors)

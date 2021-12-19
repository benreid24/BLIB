target_sources(BLIB PRIVATE
    Event.cpp
    Item.cpp
    Menu.cpp
)

add_subdirectory(EventGenerators)
add_subdirectory(Items)
add_subdirectory(Selectors)

target_sources(BLIB PRIVATE
    Event.cpp
    Item.cpp
    Menu.cpp
)

add_subdirectory(Drivers)
add_subdirectory(Items)
add_subdirectory(Selectors)

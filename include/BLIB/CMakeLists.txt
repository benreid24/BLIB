target_sources(BLIB PUBLIC
    Containers.hpp
    Engine.hpp
    Entities.hpp
    Events.hpp
    Files.hpp
    GUI.hpp
    Logging.hpp
    Media.hpp
    Menu.hpp
    Parser.hpp
    Resources.hpp
    Scripts.hpp
    Util.hpp
)

add_subdirectory(Containers)
add_subdirectory(Engine)
add_subdirectory(Entities)
add_subdirectory(Events)
add_subdirectory(Files)
add_subdirectory(GUI)
add_subdirectory(Logging)
add_subdirectory(Media)
add_subdirectory(Menu)
add_subdirectory(Parser)
add_subdirectory(Scripts)
add_subdirectory(Util)

target_sources(BLIB PUBLIC
    AI.hpp
    Audio.hpp
    Cameras.hpp
    Containers.hpp
    Components.hpp
    ECS.hpp
    Engine.hpp
    Events.hpp
    Game.hpp
    Graphics.hpp
    Input.hpp
    Interfaces.hpp
    Logging.hpp
    Parser.hpp
    Particles.hpp
    Render.hpp
    Resources.hpp
    Scripts.hpp
    Serialization.hpp
    Systems.hpp
    Tilemap.hpp
    Util.hpp
)

add_subdirectory(AI)
add_subdirectory(Audio)
add_subdirectory(Cameras)
add_subdirectory(Containers)
add_subdirectory(Components)
add_subdirectory(ECS)
add_subdirectory(Engine)
add_subdirectory(Events)
add_subdirectory(Graphics)
add_subdirectory(Input)
add_subdirectory(Interfaces)
add_subdirectory(Logging)
add_subdirectory(Math)
add_subdirectory(Parser)
add_subdirectory(Particles)
add_subdirectory(Render)
add_subdirectory(Scripts)
add_subdirectory(Serialization)
add_subdirectory(Systems)
add_subdirectory(Tilemap)
add_subdirectory(Util)

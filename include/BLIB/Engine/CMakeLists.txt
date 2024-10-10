target_sources(BLIB PUBLIC
    Configuration.hpp
    Engine.hpp
    Flags.hpp
    FrameStage.hpp
    Player.hpp
    Properties.hpp
    Property.hpp
    Settings.hpp
    State.hpp
    System.hpp
    Systems.hpp
    Window.hpp
    Worker.hpp
)

add_subdirectory(Events)
add_subdirectory(Worlds)

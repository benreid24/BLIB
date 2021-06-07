target_sources(BLIB PUBLIC
    Action.hpp
    GUI.hpp
    RawEvent.hpp
    Signal.hpp
)

add_subdirectory(Dialogs)
add_subdirectory(Elements)
add_subdirectory(Packers)
add_subdirectory(Renderers)

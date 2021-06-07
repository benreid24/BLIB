target_sources(BLIB PRIVATE
    Action.cpp
    GUI.cpp
    RawEvent.cpp
)

add_subdirectory(Data)
add_subdirectory(Dialogs)
add_subdirectory(Elements)
add_subdirectory(Packers)
add_subdirectory(Renderers)

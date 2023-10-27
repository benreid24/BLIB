target_sources(BLIB PRIVATE
    Event.cpp
    GUI.cpp
    RenderSettings.cpp
)

add_subdirectory(Data)
add_subdirectory(Dialogs)
add_subdirectory(Elements)
add_subdirectory(Packers)
add_subdirectory(Renderer)

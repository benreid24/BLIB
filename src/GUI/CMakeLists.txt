target_sources(BLIB PRIVATE
    Action.cpp
    GUI.cpp
    Signal.cpp
)

add_subdirectory(Data)
add_subdirectory(Elements)
add_subdirectory(Packers)
add_subdirectory(Renderers)
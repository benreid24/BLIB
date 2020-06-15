target_sources(BLIB PUBLIC
    Files.hpp
    GUI.hpp
    Media.hpp
    Parser.hpp
    Resources.hpp
    Scripts.hpp
    Util.hpp
)

add_subdirectory(Files)
add_subdirectory(GUI)
add_subdirectory(Media)
add_subdirectory(Parser)
add_subdirectory(Scripts)
add_subdirectory(Util)
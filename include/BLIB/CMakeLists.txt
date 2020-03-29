target_sources(BLIB PUBLIC
    Files.hpp
    Media.hpp
    Parser.hpp
    Resources.hpp
    Scripts.hpp
    Util.hpp
)

add_subdirectory(Files)
add_subdirectory(Media)
add_subdirectory(Parser)
add_subdirectory(Scripts)
add_subdirectory(Util)
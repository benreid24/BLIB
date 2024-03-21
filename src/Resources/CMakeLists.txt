target_sources(BLIB PRIVATE
    Bundler.cpp
    FileSystem.cpp
    GarbageCollector.cpp
    ResourceManager.cpp
    State.cpp
)

add_subdirectory(Bundling)

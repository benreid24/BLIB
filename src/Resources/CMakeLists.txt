target_sources(BLIB PRIVATE
    Bundler.cpp
    FileSystem.cpp
    GarbageCollector.cpp
    ResourceManager.cpp
)

add_subdirectory(Bundling)

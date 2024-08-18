target_sources(BLIB PUBLIC
    Bundler.hpp
    FileSystem.hpp
    GarbageCollector.hpp
    Loader.hpp
    Ref.hpp
    Resource.hpp
    ResourceManager.hpp
    State.hpp
)

add_subdirectory(Bundling)

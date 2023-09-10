target_sources(BLIB PUBLIC
    Cleaner.hpp
    ComponentMask.hpp
    ComponentPool.hpp
    ComponentSet.hpp
    ComponentSetImpl.hpp
    Entity.hpp
    Events.hpp
    ParentGraph.hpp
    Registry.hpp
    StableHandle.hpp
    Tags.hpp
    TagsImpl.hpp
    View.hpp
    ViewImpl.hpp
)

add_subdirectory(Traits)

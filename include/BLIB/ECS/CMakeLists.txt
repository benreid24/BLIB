target_sources(BLIB PUBLIC
    Cleaner.hpp
    ComponentMask.hpp
    ComponentPool.hpp
    ComponentSet.hpp
    ComponentSetImpl.hpp
    DependencyGraph.hpp
    Entity.hpp
    Events.hpp
    ParentDestructionBehavior.hpp
    ParentGraph.hpp
    Registry.hpp
    Tags.hpp
    TagsImpl.hpp
    View.hpp
    ViewImpl.hpp
)

add_subdirectory(Traits)

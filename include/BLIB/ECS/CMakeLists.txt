target_sources(BLIB PUBLIC
    Cleaner.hpp
    ComponentMask.hpp
    ComponentPool.hpp
    ComponentSet.hpp
    ComponentSetImpl.hpp
    DependencyGraph.hpp
    Entity.hpp
    EntityBacked.hpp
    Events.hpp
    Flags.hpp
    ParentDestructionBehavior.hpp
    ParentGraph.hpp
    Registry.hpp
    Tags.hpp
    TagsImpl.hpp
    Transaction.hpp
    TransactionImpl.hpp
    View.hpp
    ViewImpl.hpp
)

add_subdirectory(Traits)

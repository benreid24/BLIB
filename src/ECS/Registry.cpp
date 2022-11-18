#include <BLIB/ECS.hpp>

namespace bl
{
namespace ecs
{
unsigned int ComponentPoolBase::nextComponentIndex = 0;

Registry::Registry(std::size_t ec)
: maxEntities(ec)
, entityAllocator(ec)
, entityMasks(ec, ComponentMask::EmptyMask) {
    componentPools.reserve(ComponentPoolBase::MaximumComponentCount);
    views.reserve(32);
}

Entity Registry::createEntity() {
    std::lock_guard lock(entityLock);

    if (!entityAllocator.available()) {
        BL_LOG_CRITICAL << "Out of entity storage. Increase entity allocation in engine settings";
        std::exit(1);
    }

    Entity ent       = entityAllocator.allocate();
    entityMasks[ent] = ComponentMask::EmptyMask;
    bl::event::Dispatcher::dispatch<event::EntityCreated>({ent});
    return ent;
}

bool Registry::entityExists(Entity ent) const { return entityAllocator.isAllocated(ent); }

void Registry::destroyEntity(Entity ent) {
    std::lock_guard lock(entityLock);
    const ComponentMask::Value mask = entityMasks[ent];

    bl::event::Dispatcher::dispatch<event::EntityDestroyed>({ent});
    for (auto& view : views) {
        if (ComponentMask::completelyContains(mask, view->mask)) { view->removeEntity(ent); }
    }

    for (ComponentPoolBase* pool : componentPools) {
        if (ComponentMask::has(mask, pool->ComponentIndex)) { pool->remove(ent); }
    }

    entityAllocator.release(ent);
    entityMasks[ent] = ComponentMask::EmptyMask;
}

void Registry::destroyAllEntities() {
    std::lock_guard lock(entityLock);

    // send entity events
    for (Entity ent = 0; ent <= entityAllocator.highestId(); ++ent) {
        if (entityAllocator.isAllocated(ent)) {
            bl::event::Dispatcher::dispatch<event::EntityDestroyed>({ent});
        }
    }

    // clear pools
    for (ComponentPoolBase* pool : componentPools) { pool->clear(); }

    // reset entity id management
    for (auto maskIt = entityMasks.begin(); maskIt != entityMasks.end(); ++maskIt) {
        *maskIt = ComponentMask::EmptyMask;
    }
    entityAllocator.releaseAll();

    // clear views
    for (auto& view : views) { view->clearAndRefresh(*this); }
}

} // namespace ecs
} // namespace bl

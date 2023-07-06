#include <BLIB/ECS.hpp>

namespace bl
{
namespace ecs
{
Registry::Registry()
: entityAllocator(DefaultCapacity)
, entityMasks(DefaultCapacity, ComponentMask::EmptyMask) {
    componentPools.reserve(ComponentMask::MaxComponentTypeCount);
    views.reserve(32);
    bl::event::Dispatcher::subscribe(this);
}

Entity Registry::createEntity() {
    std::lock_guard lock(entityLock);

    const Entity ent = entityAllocator.allocate();
    if (ent + 1 >= entityMasks.size()) { entityMasks.resize(ent + 1, ComponentMask::EmptyMask); }
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
    for (Entity ent = 0; ent < entityAllocator.poolSize(); ++ent) {
        if (entityAllocator.isAllocated(ent)) {
            bl::event::Dispatcher::dispatch<event::EntityDestroyed>({ent});
        }
    }

    // clear pools
    for (ComponentPoolBase* pool : componentPools) { pool->clear(); }

    // reset entity id management
    entityAllocator.releaseAll();
    std::fill(entityMasks.begin(), entityMasks.end(), ComponentMask::EmptyMask);

    // clear views
    for (auto& view : views) { view->clearAndRefresh(); }
}

void Registry::observe(const event::ComponentPoolResized& resize) {
    for (auto& view : views) {
        if (ComponentMask::has(view->mask, resize.poolIndex)) { view->needsAddressReload = true; }
    }
}

} // namespace ecs
} // namespace bl

#include <BLIB/ECS.hpp>

namespace bl
{
namespace ecs
{
Registry::Registry(std::size_t ec, bl::event::Dispatcher& bus)
: maxEntities(ec)
, eventBus(bus)
, aliveEntities(ec, false)
, entityMasks(ec, ComponentMask::EmptyMask)
, freeEntities(ec)
, nextEntity(0) {
    componentPools.reserve(ComponentPoolBase::MaximumComponentCount);
    views.reserve(32);
}

Entity Registry::createEntity() {
    std::lock_guard lock(entityLock);

    Entity ent = nextEntity;
    if (!freeEntities.empty()) {
        ent = freeEntities.front();
        freeEntities.pop();
    }
    else {
        ++nextEntity;
    }

    aliveEntities[ent] = true;
    entityMasks[ent]   = ComponentMask::EmptyMask;
    eventBus.dispatch<event::EntityCreated>({ent});
    return ent;
}

bool Registry::entityExists(Entity ent) const { return aliveEntities[ent]; }

void Registry::destroyEntity(Entity ent) {
    std::lock_guard lock(entityLock);
    const ComponentMask::Value mask = entityMasks[ent];

    eventBus.dispatch<event::EntityDestroyed>({ent});
    for (auto& view : views) {
        if (ComponentMask::overlaps(view->mask, mask)) { view->removeEntity(ent); }
    }

    for (ComponentPoolBase* pool : componentPools) {
        if (ComponentMask::has(mask, pool->ComponentIndex)) { pool->remove(ent, eventBus); }
    }

    aliveEntities[ent] = false;
    entityMasks[ent]   = ComponentMask::EmptyMask;

    if (ent == nextEntity - 1) {
        do { --nextEntity; } while (!aliveEntities[nextEntity - 1]);
    }
}

void Registry::destroyAllEntities() {
    std::lock_guard lock(entityLock);

    // send entity events
    for (Entity ent = 0; ent < nextEntity; ++ent) {
        if (aliveEntities[ent]) { eventBus.dispatch<event::EntityDestroyed>({ent}); }
    }

    // clear pools
    for (ComponentPoolBase* pool : componentPools) { pool->clear(eventBus); }

    // clear views
    for (auto& view : views) { view->clearAndRefresh(*this); }
}

} // namespace ecs
} // namespace bl

#include <BLIB/ECS.hpp>

namespace bl
{
namespace ecs
{
unsigned int ComponentPoolBase::nextComponentIndex = 0;

Registry::Registry(std::size_t ec)
: maxEntities(ec)
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

    if (ent >= maxEntities) {
        BL_LOG_CRITICAL << "Out of entity storage. Increase entity allocation in engine settings";
        std::exit(1);
    }

    aliveEntities[ent] = true;
    entityMasks[ent]   = ComponentMask::EmptyMask;
    bl::event::Dispatcher::dispatch<event::EntityCreated>({ent});
    return ent;
}

bool Registry::entityExists(Entity ent) const { return aliveEntities[ent]; }

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

    aliveEntities[ent] = false;
    entityMasks[ent]   = ComponentMask::EmptyMask;
}

void Registry::destroyAllEntities() {
    std::lock_guard lock(entityLock);

    // send entity events
    for (Entity ent = 0; ent < nextEntity; ++ent) {
        if (aliveEntities[ent]) { bl::event::Dispatcher::dispatch<event::EntityDestroyed>({ent}); }
    }

    // clear pools
    for (ComponentPoolBase* pool : componentPools) { pool->clear(); }

    // reset entity id management
    auto aliveIt = aliveEntities.begin();
    auto maskIt  = entityMasks.begin();
    while (aliveIt != aliveEntities.end() && maskIt != entityMasks.end()) {
        *aliveIt = false;
        *maskIt  = ComponentMask::EmptyMask;
        ++aliveIt;
        ++maskIt;
    }
    nextEntity = 0;
    freeEntities.clear();

    // clear views
    for (auto& view : views) { view->clearAndRefresh(*this); }
}

} // namespace ecs
} // namespace bl

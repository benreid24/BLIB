#include <BLIB/ECS.hpp>

namespace bl
{
namespace ecs
{
Registry::Registry()
: entityAllocator(DefaultCapacity)
, entityMasks(DefaultCapacity, ComponentMask::EmptyMask)
, entityVersions(DefaultCapacity, 0) {
    componentPools.reserve(ComponentMask::MaxComponentTypeCount);
    views.reserve(32);
    bl::event::Dispatcher::subscribe(this);
}

Entity Registry::createEntity() {
    std::lock_guard lock(entityLock);

    const std::uint64_t index = entityAllocator.allocate();
    std::uint64_t version     = 1;
    if (index + 1 > entityMasks.size()) {
        entityMasks.resize(index + 1, ComponentMask::EmptyMask);
        entityVersions.resize(index + 1, 1);
    }
    else {
        entityMasks[index] = ComponentMask::EmptyMask;
        version            = ++entityVersions[index];
    }

    const Entity ent = IdUtil::composeEntity(index, version);
    bl::event::Dispatcher::dispatch<event::EntityCreated>({ent});
    return ent;
}

bool Registry::entityExists(Entity ent) const {
    const std::uint64_t index = IdUtil::getEntityIndex(ent);
    return entityAllocator.isAllocated(index) &&
           entityVersions[index] == IdUtil::getEntityVersion(ent);
}

void Registry::destroyEntity(Entity ent) {
    std::lock_guard lock(entityLock);
    const std::uint32_t index            = IdUtil::getEntityIndex(ent);
    const ComponentMask::SimpleMask mask = entityMasks[index];

    bl::event::Dispatcher::dispatch<event::EntityDestroyed>({ent});
    for (auto& view : views) {
        if (view->mask.passes(mask)) { view->removeEntity(ent); }
    }

    for (ComponentPoolBase* pool : componentPools) {
        if (ComponentMask::has(mask, pool->ComponentIndex)) { pool->remove(ent); }
    }

    entityAllocator.release(index);
    entityMasks[index] = ComponentMask::EmptyMask;
}

void Registry::destroyAllEntities() {
    std::lock_guard lock(entityLock);

    // send entity events
    for (std::uint32_t index = 0; index < entityAllocator.poolSize(); ++index) {
        if (entityAllocator.isAllocated(index)) {
            bl::event::Dispatcher::dispatch<event::EntityDestroyed>(
                {IdUtil::composeEntity(index, entityVersions[index])});
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
        if (view->mask.contains(resize.poolIndex)) { view->needsAddressReload = true; }
    }
}

} // namespace ecs
} // namespace bl

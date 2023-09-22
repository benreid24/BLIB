#include <BLIB/ECS.hpp>

namespace bl
{
namespace ecs
{
Registry::Registry()
: entityAllocator(DefaultCapacity)
, entityMasks(DefaultCapacity, ComponentMask::EmptyMask)
, entityVersions(DefaultCapacity, 0)
, parentGraph(DefaultCapacity)
, dependencyGraph(DefaultCapacity) {
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
    std::lock_guard lock(entityLock);
    return entityExistsLocked(ent);
}

bool Registry::entityExistsLocked(Entity ent) const {
    const std::uint64_t index = IdUtil::getEntityIndex(ent);
    return entityAllocator.isAllocated(index) &&
           entityVersions[index] == IdUtil::getEntityVersion(ent);
}

bool Registry::destroyEntity(Entity start) {
    std::lock_guard lock(entityLock);

    // check if we can remove due to dependencies
    if (dependencyGraph.hasDependencies(start)) {
        markEntityForRemoval(start);
        return false;
    }

    // determine list of entities to remove due to parenting and dependencies
    std::vector<Entity> toRemove;
    std::vector<Entity> toVisit;
    toRemove.reserve(16);
    toVisit.reserve(16);
    toVisit.emplace_back(start);

    while (!toVisit.empty()) {
        const Entity ent = toVisit.back();
        toVisit.pop_back();
        toRemove.emplace_back(ent);

        // add marked dependencies
        for (Entity resource : dependencyGraph.getResources(ent)) {
            dependencyGraph.removeDependency(resource, ent);
            if (!dependencyGraph.hasDependencies(resource)) {
                const std::uint32_t i = IdUtil::getEntityIndex(resource);
                if (i < markedForRemoval.size() && markedForRemoval[i]) {
                    toVisit.emplace_back(resource);
                }
            }
        }

        // add children
        for (Entity child : parentGraph.getChildren(ent)) { toVisit.emplace_back(child); }
    }

    // remove all discovered entities
    for (const Entity ent : toRemove) {
        const std::uint32_t index            = IdUtil::getEntityIndex(ent);
        const ComponentMask::SimpleMask mask = entityMasks[index];

        // notify external and remove from views
        bl::event::Dispatcher::dispatch<event::EntityDestroyed>({ent});
        for (auto& view : views) {
            if (view->mask.passes(mask)) { view->removeEntity(ent); }
        }

        // destroy components
        for (ComponentPoolBase* pool : componentPools) {
            if (ComponentMask::has(mask, pool->ComponentIndex)) { pool->remove(ent); }
        }

        // reset metadata
        entityAllocator.release(index);
        entityMasks[index] = ComponentMask::EmptyMask;

        // remove parenting info
        parentGraph.removeEntity(ent);

        // remove dependency info
        if (index < markedForRemoval.size()) { markedForRemoval[index] = false; }
    }

    return true;
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

    // reset relationships
    parentGraph.reset();
    dependencyGraph.clear();
    std::fill(markedForRemoval.begin(), markedForRemoval.end(), false);

    // clear views
    for (auto& view : views) { view->clearAndRefresh(); }
}

void Registry::setEntityParent(Entity child, Entity parent) {
    std::lock_guard lock(entityLock);
    const std::uint32_t ic = IdUtil::getEntityIndex(child);
    const std::uint32_t ip = IdUtil::getEntityIndex(child);
    if (!entityAllocator.isAllocated(ic) || !entityAllocator.isAllocated(ip)) {
        BL_LOG_WARN << "Tried to parent bad entities: " << child << " <- " << parent;
        return;
    }

    const ComponentMask mask{.required = entityMasks[ic]};
    parentGraph.setParent(child, parent);
    for (ComponentPoolBase* pool : componentPools) {
        if (mask.contains(pool->ComponentIndex)) { pool->onParentSet(child, parent); }
    }
}

void Registry::removeEntityParent(Entity child) {
    std::lock_guard lock(entityLock);
    const Entity parent = parentGraph.getParent(child);
    if (parent == InvalidEntity) { return; }
    const std::uint32_t ic = IdUtil::getEntityIndex(child);
    const std::uint32_t ip = IdUtil::getEntityIndex(child);
    if (!entityAllocator.isAllocated(ic) || !entityAllocator.isAllocated(ip)) {
        BL_LOG_WARN << "Tried to un-parent bad entities: " << child << " <- " << parent;
        return;
    }

    const ComponentMask mask{.required = entityMasks[ic]};
    parentGraph.unParent(child);
    for (ComponentPoolBase* pool : componentPools) {
        if (mask.contains(pool->ComponentIndex)) { pool->onParentRemove(parent, child); }
    }
}

void Registry::addDependency(Entity resource, Entity user) {
    std::lock_guard lock(entityLock);

    if (!entityExistsLocked(resource) || !entityExistsLocked(user)) {
        BL_LOG_ERROR << "Tried to add bad dependency (entities do not exist). Resource: "
                     << resource << ". User: " << user;
        return;
    }

    dependencyGraph.addDependency(resource, user);
}

void Registry::removeDependency(Entity resource, Entity user) {
    {
        std::lock_guard lock(entityLock);

        if (!entityExistsLocked(resource) || !entityExistsLocked(user)) {
            BL_LOG_ERROR << "Tried to remove bad dependency (entities do not exist). Resource: "
                         << resource << ". User: " << user;
            return;
        }

        dependencyGraph.removeDependency(resource, user);
    }

    // remove if marked
    const std::uint32_t i = IdUtil::getEntityIndex(resource);
    if (i < markedForRemoval.size() && markedForRemoval[i]) {
        markedForRemoval[i] = false;
        destroyEntity(resource);
    }
}

void Registry::removeDependencyAndDestroyIfPossible(Entity resource, Entity user) {
    removeDependency(resource, user);
    if (entityExists(resource)) {
        if (!isDependedOn(resource)) { destroyEntity(resource); }
        else { markEntityForRemoval(resource); }
    }
}

bool Registry::isDependedOn(Entity resource) const {
    std::lock_guard lock(entityLock);
    return dependencyGraph.hasDependencies(resource);
}

void Registry::markEntityForRemoval(Entity ent) {
    const std::uint32_t i = IdUtil::getEntityIndex(ent);
    if (markedForRemoval.size() <= i) { markedForRemoval.resize(i + 1, false); }
    markedForRemoval[i] = true;
}

void Registry::observe(const event::ComponentPoolResized& resize) {
    for (auto& view : views) {
        if (view->mask.contains(resize.poolIndex)) { view->needsAddressReload = true; }
    }
}

} // namespace ecs
} // namespace bl

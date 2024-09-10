#include <BLIB/ECS.hpp>

namespace bl
{
namespace ecs
{
Registry::Registry()
: entityAllocator(DefaultCapacity)
, entityMasks(DefaultCapacity, ComponentMask::EmptyMask)
, entityFlags(DefaultCapacity, Flags::None)
, entityVersions(DefaultCapacity, 0)
, entityWorlds(DefaultCapacity, 0)
, parentGraph(DefaultCapacity)
, parentDestructionBehaviors(DefaultCapacity, ParentDestructionBehavior::DestroyedWithParent)
, dependencyGraph(DefaultCapacity) {
    componentPools.reserve(ComponentMask::MaxComponentTypeCount);
    views.reserve(32);
    deletionState.toRemove.reserve(32);
}

Entity Registry::createEntity(unsigned int worldIndex, Flags flags) {
    std::lock_guard lock(entityLock);

    const std::uint64_t index = entityAllocator.allocate();
    std::uint64_t version     = 1;
    if (index + 1 > entityMasks.size()) {
        entityMasks.resize(index + 1, ComponentMask::EmptyMask);
        entityVersions.resize(index + 1, 1);
        parentDestructionBehaviors.resize(index + 1,
                                          ParentDestructionBehavior::DestroyedWithParent);
        entityFlags.resize(index + 1, Flags::None);
        entityWorlds.resize(index + 1, 0);
    }
    else {
        entityMasks[index]                = ComponentMask::EmptyMask;
        version                           = ++entityVersions[index];
        parentDestructionBehaviors[index] = ParentDestructionBehavior::DestroyedWithParent;
    }
    entityFlags[index]  = flags;
    entityWorlds[index] = worldIndex;

    const Entity ent(index, version, flags, worldIndex);
    bl::event::Dispatcher::dispatch<event::EntityCreated>({ent});
    return ent;
}

bool Registry::entityExists(Entity ent) const {
    std::lock_guard lock(entityLock);
    return entityExistsLocked(ent);
}

bool Registry::entityExistsLocked(Entity ent) const {
    const std::uint64_t index = ent.getIndex();
    return entityAllocator.isAllocated(index) && entityVersions[index] == ent.getVersion();
}

bool Registry::destroyEntity(Entity start) {
    if (!entityExists(start)) { return false; }
    std::unique_lock deleteLock(deletionState.mutex);

    // increment version to make entityExists() return false for this entity
    ++entityVersions[start.getIndex()];

    // if we are already traversing then just add to queue
    if (!deletionState.toVisit.empty()) {
        deletionState.toVisit.emplace_back(start);
        return true;
    }
    else { return queueEntityDestroy(start); }
}

bool Registry::queueEntityDestroy(Entity start) {
    // check if we can remove due to dependencies
    if (dependencyGraph.hasDependencies(start)) {
        markEntityForRemoval(start);
        return false;
    }

    // determine list of entities to remove due to parenting and dependencies
    std::unique_lock deleteLock(deletionState.mutex);
    std::vector<Entity>& toRemove   = deletionState.toRemove;
    std::vector<Entity>& toVisit    = deletionState.toVisit;
    std::vector<Entity>& toUnparent = deletionState.toUnparent;
    toRemove.reserve(16);
    toVisit.reserve(16);
    toVisit.emplace_back(start);

    while (!toVisit.empty()) {
        const Entity visiting = toVisit.back();
        toVisit.pop_back();
        toRemove.emplace_back(visiting);

        // send events now
        const std::uint32_t index            = visiting.getIndex();
        const ComponentMask::SimpleMask mask = entityMasks[index];
        bl::event::Dispatcher::dispatch<event::EntityDestroyed>({visiting});
        for (ComponentPoolBase* pool : componentPools) {
            if (ComponentMask::has(mask, pool->ComponentIndex)) {
                pool->fireRemoveEventOnly(visiting);
            }
        }

        // add marked dependencies
        for (Entity resource : dependencyGraph.getResources(visiting)) {
            dependencyGraph.removeDependency(resource, visiting);
            if (!dependencyGraph.hasDependencies(resource)) {
                const std::uint32_t i = resource.getIndex();
                if (i < markedForRemoval.size() && markedForRemoval[i]) {
                    toVisit.emplace_back(resource);
                }
            }
        }

        // add children
        for (Entity child : parentGraph.getChildren(visiting)) {
            const std::uint32_t j = child.getIndex();
            switch (parentDestructionBehaviors[j]) {
            case ParentDestructionBehavior::DestroyedWithParent:
                toVisit.emplace_back(child);
                break;
            case ParentDestructionBehavior::OrphanedByParent:
                toUnparent.reserve(16);
                toUnparent.emplace_back(child);
                break;
            }
        }
    }

    return true;
}

void Registry::doEntityDestroyLocked(Entity ent) {
    removeEntityParentLocked(ent);

    const std::uint32_t index            = ent.getIndex();
    const ComponentMask::SimpleMask mask = entityMasks[index];

    // reset metadata
    entityAllocator.release(index);
    entityMasks[index]  = ComponentMask::EmptyMask;
    entityFlags[index]  = Flags::None;
    entityWorlds[index] = 0;

    // remove from views
    for (auto& view : views) {
        if (view->mask.passes(mask)) { view->removeEntity(ent); }
    }

    // destroy components
    for (ComponentPoolBase* pool : componentPools) {
        if (ComponentMask::has(mask, pool->ComponentIndex)) { pool->remove(ent, false); }
    }

    // remove parenting info
    parentGraph.removeEntity(ent);

    // remove dependency info
    if (index < markedForRemoval.size()) { markedForRemoval[index] = false; }
}

void Registry::flushDeletions() {
    std::unique_lock lock(entityLock);
    std::unique_lock queueLock(deletionState.mutex);

    // unparent orphans
    for (Entity child : deletionState.toUnparent) { removeEntityParentLocked(child); }
    deletionState.toUnparent.clear();

    // destroy queued components
    for (auto& pool : componentPools) { pool->flushRemovals(); }

    // destroy queued entities
    for (const Entity ent : deletionState.toRemove) { doEntityDestroyLocked(ent); }
    deletionState.toRemove.clear();
}

unsigned int Registry::destroyAllEntitiesWithFlags(Flags flags) {
    std::lock_guard lock(entityLock);

    unsigned int destroyed = 0;
    for (std::uint32_t i = 0; i < entityFlags.size(); ++i) {
        if (entityAllocator.isAllocated(i)) {
            const Flags f = entityFlags[i];
            if ((f & flags) != 0) {
                queueEntityDestroy(Entity(i, entityVersions[i], f));
                ++destroyed;
            }
        }
    }
    return destroyed;
}

unsigned int Registry::destroyAllWorldEntities() {
    return destroyAllEntitiesWithFlags(Flags::WorldObject);
}

unsigned int Registry::destroyEntitiesInWorld(unsigned int worldIndex) {
    std::lock_guard lock(entityLock);

    unsigned int destroyed = 0;
    for (std::uint32_t i = 0; i < entityWorlds.size(); ++i) {
        if (entityAllocator.isAllocated(i) && entityWorlds[i] == worldIndex) {
            queueEntityDestroy(Entity(i, entityVersions[i], entityFlags[i], worldIndex));
            ++destroyed;
        }
    }
    return destroyed;
}

void Registry::destroyAllEntities() {
    std::lock_guard lock(entityLock);
    std::lock_guard queueLock(deletionState.mutex);

    // send entity events
    for (std::uint32_t index = 0; index < entityAllocator.poolSize(); ++index) {
        if (entityAllocator.isAllocated(index)) {
            bl::event::Dispatcher::dispatch<event::EntityDestroyed>(
                {Entity(index, entityVersions[index])});
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

    // remove parent first
    removeEntityParentLocked(child);

    // check both valid
    const std::uint32_t ic = child.getIndex();
    const std::uint32_t ip = parent.getIndex();
    if (!entityAllocator.isAllocated(ic) || !entityAllocator.isAllocated(ip)) {
        BL_LOG_WARN << "Tried to parent bad entities: " << child << " <- " << parent;
        return;
    }

    // assign parent and update components
    const ComponentMask mask{.required = entityMasks[ic]};
    parentGraph.setParent(child, parent);
    for (ComponentPoolBase* pool : componentPools) {
        if (mask.contains(pool->ComponentIndex)) { pool->onParentSet(child, parent); }
    }

    bl::event::Dispatcher::dispatch<event::EntityParentSet>({parent, child});
}

void Registry::removeEntityParent(Entity child) {
    std::lock_guard lock(entityLock);
    removeEntityParentLocked(child);
}

void Registry::removeEntityParentLocked(Entity child) {
    const Entity parent = parentGraph.getParent(child);
    if (parent == InvalidEntity) { return; }
    const std::uint32_t ic = child.getIndex();
    const std::uint32_t ip = parent.getIndex();
    if (!entityAllocator.isAllocated(ic) || !entityAllocator.isAllocated(ip)) {
        BL_LOG_WARN << "Tried to un-parent bad entities: " << child << " <- " << parent;
        return;
    }

    const ComponentMask mask{.required = entityMasks[ic]};
    parentGraph.unParent(child);
    for (ComponentPoolBase* pool : componentPools) {
        if (mask.contains(pool->ComponentIndex)) { pool->onParentRemove(parent, child); }
    }

    bl::event::Dispatcher::dispatch<event::EntityParentRemoved>({child});
}

void Registry::addDependency(Entity resource, Entity user) {
    std::lock_guard lock(entityLock);

    if (!entityExistsLocked(resource) || !entityExistsLocked(user)) {
        BL_LOG_ERROR << "Tried to add bad dependency (entities do not exist). Resource: "
                     << resource << ". User: " << user;
        return;
    }

    dependencyGraph.addDependency(resource, user);

    bl::event::Dispatcher::dispatch<event::EntityDependencyAdded>({resource, user});
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

    bl::event::Dispatcher::dispatch<event::EntityDependencyRemoved>({resource, user});

    // remove if marked
    const std::uint32_t i = resource.getIndex();
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
    const std::uint64_t i = ent.getIndex();
    if (markedForRemoval.size() <= i) { markedForRemoval.resize(i + 1, false); }
    markedForRemoval[i] = true;
}

void Registry::setEntityParentDestructionBehavior(Entity entity,
                                                  ParentDestructionBehavior behavior) {
    const std::uint32_t i = entity.getIndex();
    if (i < parentDestructionBehaviors.size()) { parentDestructionBehaviors[i] = behavior; }
    else { BL_LOG_ERROR << "Cannot set behavior on invalid entity: " << entity; }
}

} // namespace ecs
} // namespace bl

#include <BLIB/ECS.hpp>

namespace bl
{
namespace ecs
{
Registry::Registry()
: entityAllocator(DefaultCapacity)
, entityMasks(DefaultCapacity, ComponentMask::EmptyMask)
, entityFlags(DefaultCapacity, Flags::None)
, entityVersions(DefaultCapacity, 1)
, entityWorlds(DefaultCapacity, 0)
, parentGraph(DefaultCapacity)
, parentDestructionBehaviors(DefaultCapacity, ParentDestructionBehavior::DestroyedWithParent)
, dependencyGraph(DefaultCapacity) {
    componentPools.reserve(ComponentMask::MaxComponentTypeCount);
    views.reserve(32);
    deletionState.toRemove.reserve(32);
    emitter.connect(signalChannel);
}

Entity Registry::createEntity(unsigned int worldIndex, Flags flags) {
    return createEntity(worldIndex, flags, Transaction<tx::EntityWrite>(*this));
}

Entity Registry::createEntity(unsigned int worldIndex, Flags flags,
                              const Transaction<tx::EntityWrite>&) {
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
        version                           = entityVersions[index];
        parentDestructionBehaviors[index] = ParentDestructionBehavior::DestroyedWithParent;
    }

    entityFlags[index]  = flags;
    entityWorlds[index] = worldIndex;

    const Entity ent(index, version, flags, worldIndex);
    emitter.emit<event::EntityCreated>({ent});
    return ent;
}

bool Registry::entityExists(Entity ent) const {
    return entityExists(ent, Transaction<tx::EntityRead>(const_cast<Registry&>(*this)));
}

bool Registry::entityExists(Entity entity, const Transaction<tx::EntityRead>&) const {
    return entityExistsLocked(entity);
}

bool Registry::entityExistsLocked(Entity ent) const {
    const std::uint64_t index = ent.getIndex();
    return entityAllocator.isAllocated(index) && entityVersions[index] == ent.getVersion();
}

bool Registry::destroyEntity(Entity start) {
    return destroyEntity(start, Transaction<tx::EntityRead>(*this));
}

bool Registry::destroyEntity(Entity start, const Transaction<tx::EntityRead>& tx) {
    if (!entityExists(start, tx)) { return false; }
    std::unique_lock deleteLock(deletionState.mutex);

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

        if (!entityExistsLocked(visiting)) { continue; }

        toRemove.emplace_back(visiting);

        // increment version here
        ++entityVersions[visiting.getIndex()];

        // send events now
        const std::uint32_t index            = visiting.getIndex();
        const ComponentMask::SimpleMask mask = entityMasks[index];
        emitter.emit<event::EntityDestroyed>({visiting});
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

    // unparent orphans
    std::unique_lock parentLock(entityLock);
    for (Entity child : deletionState.toUnparent) { removeEntityParentLocked(child, false); }
    deletionState.toUnparent.clear();

    return true;
}

void Registry::doEntityDestroyLocked(Entity ent) {
    removeEntityParentLocked(ent, true);

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

    // destroy queued components
    for (auto& pool : componentPools) { pool->flushRemovals(); }

    // destroy queued entities
    for (const Entity ent : deletionState.toRemove) { doEntityDestroyLocked(ent); }
    deletionState.toRemove.clear();
}

unsigned int Registry::destroyAllEntitiesWithFlags(Flags flags) {
    return destroyAllEntitiesWithFlags(flags, Transaction<tx::EntityRead>(*this));
}

unsigned int Registry::destroyEntitiesInWorld(unsigned int worldIndex,
                                              const Transaction<tx::EntityRead>&) {
    unsigned int destroyed = 0;
    for (std::uint32_t i = 0; i < entityWorlds.size(); ++i) {
        if (entityAllocator.isAllocated(i) && entityWorlds[i] == worldIndex) {
            queueEntityDestroy(Entity(i, entityVersions[i], entityFlags[i], worldIndex));
            ++destroyed;
        }
    }
    return destroyed;
}

unsigned int Registry::destroyAllWorldEntities() {
    return destroyAllEntitiesWithFlags(Flags::WorldObject);
}

unsigned int Registry::destroyAllEntitiesWithFlags(Flags flags,
                                                   const Transaction<tx::EntityRead>&) {
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

unsigned int Registry::destroyEntitiesInWorld(unsigned int worldIndex) {
    return destroyEntitiesInWorld(worldIndex, Transaction<tx::EntityRead>(*this));
}

unsigned int Registry::destroyAllWorldEntities(const Transaction<tx::EntityRead>& transaction) {
    return destroyAllEntitiesWithFlags(Flags::WorldObject, transaction);
}

void Registry::destroyAllEntities() {
    std::lock_guard lock(entityLock);
    std::lock_guard queueLock(deletionState.mutex);

    // send entity events
    for (std::uint32_t index = 0; index < entityAllocator.poolSize(); ++index) {
        if (entityAllocator.isAllocated(index)) {
            emitter.emit<event::EntityDestroyed>({Entity(index, entityVersions[index])});
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
    setEntityParent(child, parent, Transaction<tx::EntityRead>(*this));
}

void Registry::setEntityParent(Entity child, Entity parent, const Transaction<tx::EntityRead>&) {
    // remove parent first
    removeEntityParentLocked(child, false);

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

    emitter.emit<event::EntityParentSet>({parent, child});
}

void Registry::removeEntityParent(Entity child) {
    removeEntityParent(child, Transaction<tx::EntityRead>(*this));
}

void Registry::removeEntityParent(Entity child, const Transaction<tx::EntityRead>&) {
    removeEntityParentLocked(child, false);
}

void Registry::removeEntityParentLocked(Entity child, bool fromDestroy) {
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

    emitter.emit<event::EntityParentRemoved>({child, fromDestroy});
}

void Registry::addDependency(Entity resource, Entity user) {
    addDependency(resource, user, Transaction<tx::EntityRead>(*this));
}

void Registry::addDependency(Entity resource, Entity user, const Transaction<tx::EntityRead>&) {
    if (!entityExistsLocked(resource) || !entityExistsLocked(user)) {
        BL_LOG_ERROR << "Tried to add bad dependency (entities do not exist). Resource: "
                     << resource << ". User: " << user;
        return;
    }

    dependencyGraph.addDependency(resource, user);

    emitter.emit<event::EntityDependencyAdded>({resource, user});
}

void Registry::removeDependency(Entity resource, Entity user) {
    removeDependency(resource, user, Transaction<tx::EntityRead>(*this));
}

void Registry::removeDependency(Entity resource, Entity user,
                                const Transaction<tx::EntityRead>& transaction) {
    if (!entityExistsLocked(resource) || !entityExistsLocked(user)) {
        BL_LOG_ERROR << "Tried to remove bad dependency (entities do not exist). Resource: "
                     << resource << ". User: " << user;
        return;
    }

    dependencyGraph.removeDependency(resource, user);

    emitter.emit<event::EntityDependencyRemoved>({resource, user});

    // remove if marked
    const std::uint32_t i = resource.getIndex();
    if (i < markedForRemoval.size() && markedForRemoval[i]) {
        markedForRemoval[i] = false;
        destroyEntity(resource, transaction);
    }
}

void Registry::removeDependencyAndDestroyIfPossible(Entity resource, Entity user) {
    removeDependencyAndDestroyIfPossible(resource, user, Transaction<tx::EntityRead>(*this));
}

void Registry::removeDependencyAndDestroyIfPossible(
    Entity resource, Entity user, const Transaction<tx::EntityRead>& transaction) {
    removeDependency(resource, user, transaction);
    if (entityExists(resource, transaction)) {
        if (!isDependedOn(resource, transaction)) { destroyEntity(resource, transaction); }
        else { markEntityForRemoval(resource); }
    }
}

bool Registry::isDependedOn(Entity resource) const {
    return isDependedOn(resource, Transaction<tx::EntityRead>(const_cast<Registry&>(*this)));
}

bool Registry::isDependedOn(Entity resource, const Transaction<tx::EntityRead>&) const {
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

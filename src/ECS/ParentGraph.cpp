#include <BLIB/ECS/ParentGraph.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace ecs
{
ecs::ParentGraph::ParentGraph(std::size_t capacityHint)
: parentMap(capacityHint, InvalidEntity)
, childMap(capacityHint) {}

void ParentGraph::setParent(Entity child, Entity parent) {
    ensureParentCap(child);

    const std::size_t i   = child.getIndex();
    const Entity ogParent = parentMap[i];
    if (ogParent == parent) { return; }
    else if (ogParent != InvalidEntity) { unParent(child); }

    parentMap[i] = parent;
    childMap.add(parent.getIndex(), child);

#ifdef BLIB_DEBUG
    if (checkForCycles(child)) {
        BL_LOG_CRITICAL << "Cycle created when assigning parent " << parent << " to entity "
                        << child;
        std::abort();
    }
#endif
}

void ParentGraph::unParent(Entity child) {
    const std::size_t i = child.getIndex();
    if (parentMap.size() <= i) { return; }
    if (parentMap[i] == InvalidEntity) { return; }

    const std::uint32_t j = parentMap[i].getIndex();
    parentMap[i]          = InvalidEntity;
    childMap.removeValue(j, child);
}

Entity ParentGraph::getParent(Entity child) const {
    const std::size_t i = child.getIndex();
    return i < parentMap.size() ? parentMap[i] : InvalidEntity;
}

ctr::IndexMappedList<std::uint32_t, Entity>::Range ParentGraph::getChildren(Entity parent) {
    return childMap.getValues(parent.getIndex());
}

void ParentGraph::reset() {
    std::fill(parentMap.begin(), parentMap.end(), InvalidEntity);
    childMap.clear();
}

void ParentGraph::removeEntity(Entity entity) {
    const std::size_t i = entity.getIndex();
    if (i >= parentMap.size()) { return; }

    const Entity parent = parentMap[i];
    parentMap[i]        = InvalidEntity;
    childMap.remove(entity);
    if (parent != InvalidEntity) { childMap.removeValue(parent.getIndex(), entity); }
}

void ParentGraph::ensureParentCap(Entity child) {
    const std::size_t cap = child.getIndex() + 1;
    if (cap > parentMap.size()) { parentMap.resize(cap, InvalidEntity); }
}

bool ParentGraph::checkForCycles(Entity start) {
    std::vector<bool> visited(parentMap.size(), false);
    std::vector<Entity> toVisit;
    toVisit.reserve(32);
    toVisit.emplace_back(start);

    while (!toVisit.empty()) {
        const Entity ent = toVisit.back();
        toVisit.pop_back();
        const std::uint32_t i = ent.getIndex();

        if (i >= visited.size()) { visited.resize(i + 1, false); }
        if (visited[i]) { return true; }
        visited[i] = true;

        const Entity parent = getParent(ent);
        if (parent != InvalidEntity) { toVisit.emplace_back(parent); }
    }

    return false;
}

} // namespace ecs
} // namespace bl

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

    const std::size_t i   = IdUtil::getEntityIndex(child);
    const Entity ogParent = parentMap[i];
    if (ogParent == parent) { return; }
    else if (ogParent != InvalidEntity) { unParent(child); }

    parentMap[i] = parent;
    childMap.add(IdUtil::getEntityIndex(parent), child);

#ifdef BLIB_DEBUG
    if (checkForCycles(child)) {
        BL_LOG_CRITICAL << "Cycle created when assigning parent " << parent << " to entity "
                        << child;
        std::abort();
    }
#endif
}

void ParentGraph::unParent(Entity child) {
    const std::size_t i = IdUtil::getEntityIndex(child);
    if (parentMap.size() <= i) { return; }
    if (parentMap[i] == InvalidEntity) { return; }

    const std::uint32_t j = IdUtil::getEntityIndex(parentMap[i]);
    parentMap[i]          = InvalidEntity;
    childMap.removeValue(j, child);
}

Entity ParentGraph::getParent(Entity child) {
    const std::size_t i = IdUtil::getEntityIndex(child);
    return i < parentMap.size() ? parentMap[i] : InvalidEntity;
}

ctr::IndexMappedList<std::uint32_t, Entity>::Range ParentGraph::getChildren(Entity parent) {
    return childMap.getValues(IdUtil::getEntityIndex(parent));
}

void ParentGraph::ensureParentCap(Entity child) {
    const std::size_t cap = IdUtil::getEntityIndex(child) + 1;
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
        const std::uint32_t i = IdUtil::getEntityIndex(ent);

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

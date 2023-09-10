#include <BLIB/ECS/DependencyGraph.hpp>

namespace bl
{
namespace ecs
{
ecs::DependencyGraph::DependencyGraph(std::size_t capacityHint)
: parentsToChildren(capacityHint)
, childrenToParents(capacityHint) {}

void DependencyGraph::addDependency(Entity parent, Entity child) {
    parentsToChildren.add(IdUtil::getEntityIndex(parent), child);
    childrenToParents.add(IdUtil::getEntityIndex(child), parent);
}

bool DependencyGraph::hasDependencies(Entity parent) const {
    return parentsToChildren.hasValues(IdUtil::getEntityIndex(parent));
}

void DependencyGraph::removeDependency(Entity parent, Entity child) {
    parentsToChildren.removeValue(IdUtil::getEntityIndex(parent), child);
    childrenToParents.removeValue(IdUtil::getEntityIndex(child), parent);
}

void DependencyGraph::clear() {
    parentsToChildren.clear();
    childrenToParents.clear();
}

ctr::IndexMappedList<std::uint32_t, Entity>::Range DependencyGraph::getResources(Entity child) {
    return childrenToParents.getValues(IdUtil::getEntityIndex(child));
}

} // namespace ecs
} // namespace bl

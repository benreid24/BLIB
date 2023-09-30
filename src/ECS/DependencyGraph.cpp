#include <BLIB/ECS/DependencyGraph.hpp>

namespace bl
{
namespace ecs
{
ecs::DependencyGraph::DependencyGraph(std::size_t capacityHint)
: parentsToChildren(capacityHint)
, childrenToParents(capacityHint) {}

void DependencyGraph::addDependency(Entity parent, Entity child) {
    parentsToChildren.add(parent.getIndex(), child);
    childrenToParents.add(child.getIndex(), parent);
}

bool DependencyGraph::hasDependencies(Entity parent) const {
    return parentsToChildren.hasValues(parent.getIndex());
}

void DependencyGraph::removeDependency(Entity parent, Entity child) {
    parentsToChildren.removeValue(parent.getIndex(), child);
    childrenToParents.removeValue(child.getIndex(), parent);
}

void DependencyGraph::clear() {
    parentsToChildren.clear();
    childrenToParents.clear();
}

ctr::IndexMappedList<std::uint32_t, Entity>::Range DependencyGraph::getResources(Entity child) {
    return childrenToParents.getValues(child.getIndex());
}

} // namespace ecs
} // namespace bl

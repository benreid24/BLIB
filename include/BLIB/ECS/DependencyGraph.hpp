#ifndef BLIB_ECS_DEPENDENCYGRAPH_HPP
#define BLIB_ECS_DEPENDENCYGRAPH_HPP

#include <BLIB/Containers/IndexMappedList.hpp>
#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace ecs
{
class DependencyGraph {
public:
    DependencyGraph(std::size_t capacityHint);

    void addDependency(Entity parent, Entity child);

    bool hasDependencies(Entity parent) const;

    void removeDependency(Entity parent, Entity child);

    void removeEntity(Entity child);

    void clear();

private:
    ctr::IndexMappedList<std::uint32_t, Entity> parentsToChildren;
    ctr::IndexMappedList<std::uint32_t, Entity> childrenToParents;
};

} // namespace ecs
} // namespace bl

#endif

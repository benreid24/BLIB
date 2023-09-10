#ifndef BLIB_ECS_DEPENDENCYGRAPH_HPP
#define BLIB_ECS_DEPENDENCYGRAPH_HPP

#include <BLIB/Containers/IndexMappedList.hpp>
#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace ecs
{
/**
 * @brief Helper to store and manage dependencies between components
 *
 * @ingroup ECS
 */
class DependencyGraph {
public:
    /**
     * @brief Creates the graph with the given capacity hint
     *
     * @param capacityHint The number of expected entities
     */
    DependencyGraph(std::size_t capacityHint);

    /**
     * @brief Adds a dependency between two entities
     *
     * @param parent The entity being depended on
     * @param child The entity depending on
     */
    void addDependency(Entity parent, Entity child);

    /**
     * @brief Returns whether or not the given entity has others depending on it
     *
     * @param parent The entity to check
     * @return True if dependencies are present, false otherwise
     */
    bool hasDependencies(Entity parent) const;

    /**
     * @brief Removes the given dependency
     *
     * @param parent The entity being depended on
     * @param child The entity depending on
     */
    void removeDependency(Entity parent, Entity child);

    /**
     * @brief Clears out all relationship data
     */
    void clear();

    /**
     * @brief Returns an iterable of dependencies for the given entity
     *
     * @param child The entity to get dependencies for
     * @return An iterable of entities that are depended on
     */
    ctr::IndexMappedList<std::uint32_t, Entity>::Range getResources(Entity child);

private:
    ctr::IndexMappedList<std::uint32_t, Entity> parentsToChildren;
    ctr::IndexMappedList<std::uint32_t, Entity> childrenToParents;
};

} // namespace ecs
} // namespace bl

#endif

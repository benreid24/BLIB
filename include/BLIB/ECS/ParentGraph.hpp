#ifndef BLIB_ECS_PARENTGRAPH_HPP
#define BLIB_ECS_PARENTGRAPH_HPP

#include <BLIB/Containers/IndexMappedList.hpp>
#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace ecs
{
/**
 * @brief Utility class to store and manage child <-> parent entity relationships in the ECS
 *
 * @ingroup ECS
 */
class ParentGraph {
public:
    /**
     * @brief Creates an empty graph
     *
     * @param capacityHint The expected number of entities to manage
     */
    ParentGraph(std::size_t capacityHint);

    /**
     * @brief Adds a parent <-> child association
     *
     * @param child The child entity id
     * @param parent The parent entity id
     */
    void setParent(Entity child, Entity parent);

    /**
     * @brief Removes the parent from the given entity
     *
     * @param child The child to orphan
     */
    void unParent(Entity child);

    /**
     * @brief Returns the parent for the given child
     *
     * @param child The child entity to get the parent of
     * @return The parent entity id, or InvalidEntity
     */
    Entity getParent(Entity child) const;

    /**
     * @brief Returns a range of children for the given entity
     *
     * @param parent The parent to get the children of
     * @return The range of children. May be empty
     */
    ctr::IndexMappedList<std::uint32_t, Entity>::Range getChildren(Entity parent);

    /**
     * @brief Resets the entire graph to the empty state
     */
    void reset();

    /**
     * @brief Removes the given entity from the graph. Does not update children. remove() must be
     *        called manually for all children
     *
     * @param entity The entity to remove
     */
    void removeEntity(Entity entity);

private:
    std::vector<Entity> parentMap;
    ctr::IndexMappedList<std::uint32_t, Entity> childMap;

    void ensureParentCap(Entity child);
    bool checkForCycles(Entity start);
};

} // namespace ecs
} // namespace bl

#endif

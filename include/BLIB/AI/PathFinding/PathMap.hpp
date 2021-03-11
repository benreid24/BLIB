#ifndef BLIB_AI_PATHFINDING_PATHMAP_HPP
#define BLIB_AI_PATHFINDING_PATHMAP_HPP

#include <BLIB/Entities/Entity.hpp>
#include <vector>

namespace bl
{
namespace ai
{
/**
 * @brief An interface for a traversable map meant to be navigated by a PathFinder
 *
 * @tparam TCoord The coordinate type being used for position
 * @ingroup PathFinding
 */
template<typename TCoord>
class PathMap {
public:
    virtual ~PathMap() = default;

    /**
     * @brief Returns the cost for the given entity to move from one position to another. A negative
     *        result indicates that the movement is impossible
     *
     * @param from The entity's current position
     * @param to The position the entity is trying to move to
     * @param mover The entity trying to move
     * @return int The cost to traverse the positions, or a negative number if not traversable
     */
    virtual int movementCost(const TCoord& from, const TCoord& to, entity::Entity mover) const = 0;

    /**
     * @brief Populates the result array with the list of adjacent positions
     *
     * @param node The position to get neighbors for
     * @param result A vector to populate with the adjacent nodes
     */
    virtual void getAdjacentNodes(const TCoord& node, std::vector<TCoord>& result) const = 0;
};

} // namespace ai
} // namespace bl

#endif

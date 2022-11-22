#ifndef BLIB_AI_PATHFINDING_PATHFINDER_HPP
#define BLIB_AI_PATHFINDING_PATHFINDER_HPP

#include <BLIB/AI/PathFinding/PathMap.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Util/Hashes.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <cmath>
#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>

namespace bl
{
/// Collection of utilities to aid in the development of game AI
namespace ai
{
/// Helper methods and classes for use with AI features
namespace helpers
{
/**
 * @addtogroup Helpers
 * @ingroup AI
 * @brief Helper methods and classes for use with AI features
 *
 */

/**
 * @brief Computes the manhattan distance between two points
 * @ingroup Helpers
 *
 * @tparam T The type of the underlying vector
 * @param from The origin point
 * @param to The point to calculate distance to
 * @return int The distance between the points
 */
template<typename T>
int Vector2ManhattanDist(const sf::Vector2<T>& from, const sf::Vector2<T>& to, ecs::Entity) {
    return std::abs(from.x - to.x) + std::abs(from.y - to.y);
}

/**
 * @brief Computes the real distance between two points
 * @ingroup Helpers
 *
 * @tparam T The type of the underlying vector
 * @param from The origin point
 * @param to The point to calculate distance to
 * @return int The distance between the points
 */
template<typename T>
int Vector2RealDist(const sf::Vector2<T>& from, const sf::Vector2<T>& to, ecs::Entity) {
    const int dx = from.x - to.x;
    const int dy = from.y - to.y;
    return std::sqrt(dx * dx + dy * dy);
}

/**
 * @brief Computes the manhattan distance between two points
 * @ingroup Helpers
 *
 * @tparam T The type of the underlying vector
 * @param from The origin point
 * @param to The point to calculate distance to
 * @return int The distance between the points
 */
template<typename T>
int Vector3ManhattanDist(const sf::Vector3<T>& from, const sf::Vector3<T>& to, ecs::Entity) {
    return std::abs(from.x - to.x) + std::abs(from.y - to.y) + std::abs(from.z - to.z);
}

/**
 * @brief Computes the real distance between two points
 * @ingroup Helpers
 *
 * @tparam T The type of the underlying vector
 * @param from The origin point
 * @param to The point to calculate distance to
 * @return int The distance between the points
 */
template<typename T>
int Vector3RealDist(const sf::Vector2<T>& from, const sf::Vector2<T>& to, ecs::Entity) {
    const int dx = from.x - to.x;
    const int dy = from.y - to.y;
    const int dz = from.z - to.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
} // namespace helpers

/**
 * @brief Class for calculating paths between arbitrary points in a PathMap
 *
 * @tparam TCoord The type of coordinate used to index the map
 * @tparam CoordHash A hasher for the given coordinate type
 * @ingroup PathFinding
 */
template<typename TCoord, typename CoordHash>
class PathFinder {
public:
    /// Distance estimation function signature
    using Heuristic = std::function<int(const TCoord& from, const TCoord& to, ecs::Entity mover)>;

    /**
     * @brief Builds the PathFinder with the given distance estimation heuristic
     *
     * @param heuristic The heuristic to use while searching
     */
    PathFinder(Heuristic heuristic);

    /**
     * @brief Set the heuristic to use while searching
     *
     * @param heuristic The heuristic to use while searching
     */
    void setHeuristic(Heuristic heuristic);

    /**
     * @brief Calculates the shortest path between the two points using the internal heuristic.
     *        Paths may not be optimal depending on the heuristic used
     *
     * @param start The point to start at
     * @param destination The point to end at
     * @param mover The Entity traversing the path
     * @param map The map the entity is traversing
     * @param result vector to store the calculated path in
     * @return True if a path was found, false if a path was not found
     */
    bool findPath(const TCoord& start, const TCoord& destination, ecs::Entity mover,
                  const PathMap<TCoord>& map, std::vector<TCoord>& result) const;

private:
    Heuristic heuristic;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCoord, typename CHash>
PathFinder<TCoord, CHash>::PathFinder(Heuristic h)
: heuristic(h) {}

template<typename TCoord, typename CHash>
void PathFinder<TCoord, CHash>::setHeuristic(Heuristic h) {
    heuristic = h;
}

template<typename TCoord, typename CHash>
bool PathFinder<TCoord, CHash>::findPath(const TCoord& start, const TCoord& destination,
                                         ecs::Entity mover, const PathMap<TCoord>& map,
                                         std::vector<TCoord>& result) const {
    if (start == destination) return true;

    using QueueItem         = std::pair<int, TCoord>;
    using QueueCompareSig   = std::function<bool(const QueueItem&, const QueueItem&)>;
    const auto QueueCompare = [](const QueueItem& a, const QueueItem& b) {
        return a.first > b.first;
    };

    std::priority_queue<QueueItem, std::vector<QueueItem>, QueueCompareSig> toVisit(QueueCompare);
    std::unordered_map<TCoord, TCoord, CHash> prevPositionMap; // pos index -> previous pos
    std::unordered_map<TCoord, int, CHash> positionDistances;  // pos index -> distance
    std::vector<TCoord> adjCoords;

    toVisit.emplace(heuristic(start, destination, mover), start);
    positionDistances[start] = 0;

    bool endReached = false;
    while (!toVisit.empty()) {
        const TCoord currentPos   = toVisit.top().second;
        const int currentDistance = positionDistances[currentPos];

        adjCoords.clear();
        map.getAdjacentNodes(currentPos, adjCoords);
        toVisit.pop();

        if (currentPos == destination) {
            endReached = true;
            break;
        }

        for (const TCoord& nextPos : adjCoords) {
            const int cost = map.movementCost(currentPos, nextPos, mover);
            if (cost < 0) continue;

            auto distanceIter     = positionDistances.find(nextPos);
            const int estDistance = currentDistance + cost;
            if (distanceIter == positionDistances.end() || estDistance < distanceIter->second) {
                prevPositionMap[nextPos]   = currentPos;
                positionDistances[nextPos] = estDistance;
                toVisit.emplace(estDistance + heuristic(nextPos, destination, mover), nextPos);
            }
        }
    }
    if (!endReached) return false;

    const std::size_t pathLength = positionDistances[destination];
    TCoord currentPos            = destination;
    std::size_t i                = pathLength - 1;
    result.resize(pathLength);
    while (currentPos != start) {
        result[i]  = currentPos;
        currentPos = prevPositionMap[currentPos];
        --i;
    }
    return true;
}

} // namespace ai
} // namespace bl

#endif

#ifndef BLIB_AI_PATHFINDING_PATHFINDER_HPP
#define BLIB_AI_PATHFINDING_PATHFINDER_HPP

#include <BLIB/Util/Hashes.hpp>
#include <cmath>
#include <functional>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace bl
{
/// Collection of utilities to aid in the development of game AI
namespace ai
{
/**
 * @brief Class for calculating paths between arbitrary points in a PathMap
 *
 * @tparam TNode The type of coordinate/node used to index the map
 * @tparam CoordHash A hasher for the given coordinate type
 * @ingroup AI
 */
template<typename TNode, typename CoordHash>
struct PathFinder {
    /**
     * @brief Calculates the shortest path between the two points using the map callback. Map
     *        callback signature should be void(const TNode&, std::vector<std::pair<TNode, int>>&).
     *        The vector should be populated with nodes adjacent to the given node along with the
     *        cost to move to those nodes from the given node. Distance estimator signature should
     *        be int(const TNode& from, const TNode& to)
     *
     * @tparam TMapCallback Callback to get adjacent nodes and their movement cost
     * @tparam TDistFunc Callback to estimate distance between two non-adjacent nodes
     * @param start The point to start at
     * @param destination The point to end at
     * @param adjacentNodeCb Callback to get travel nodes for a given node
     * @param distCb Callback to estimate distance between two non-adjacent nodes
     * @param result The vector to store the calculated path in
     * @return True if a path was found, false if a path was not found
     */
    template<typename TMapCallback, typename TDistFunc>
    static bool findPath(const TNode& start, const TNode& destination,
                         TMapCallback&& adjacentNodeCb, TDistFunc&& distCb,
                         std::vector<TNode>& result);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TNode, typename CHash>
template<typename TMapCb, typename TDistFunc>
bool PathFinder<TNode, CHash>::findPath(const TNode& start, const TNode& destination,
                                        TMapCb&& nodeCb, TDistFunc&& distCb,
                                        std::vector<TNode>& result) {
    static_assert(
        std::is_invocable<TMapCb, const TNode&, std::vector<std::pair<TNode, int>>&>::value,
        "Adjacent node callback should have signature: void(const TNode& current, "
        "std::vector<std::pair<TNode, int>>&");
    static_assert(
        std::is_invocable<TDistFunc, const TNode&, const TNode&>::value,
        "Distance function should have signature int(const TNode& from, const TNode& to)");
    static_assert(std::is_same_v<std::invoke_result_t<TDistFunc, const TNode&, const TNode&>, int>,
                  "Distance function should return an int");

    if (start == destination) return true;

    using QueueItem         = std::pair<int, TNode>;
    const auto QueueCompare = [](const QueueItem& a, const QueueItem& b) {
        return a.first > b.first;
    };

    std::priority_queue<QueueItem, std::vector<QueueItem>, decltype(QueueCompare)> toVisit(
        QueueCompare);
    std::unordered_map<TNode, TNode, CHash> prevPositionMap; // pos index -> previous pos
    std::unordered_map<TNode, int, CHash> positionDistances; // pos index -> distance
    std::vector<std::pair<TNode, int>> adjCoords;

    toVisit.emplace(distCb(start, destination), start);
    positionDistances[start] = 0;

    bool endReached = false;
    while (!toVisit.empty()) {
        const TNode currentPos    = toVisit.top().second;
        const int currentDistance = positionDistances[currentPos];

        adjCoords.clear();
        nodeCb(currentPos, adjCoords);
        toVisit.pop();

        if (currentPos == destination) {
            endReached = true;
            break;
        }

        for (const std::pair<TNode, int>& nextPos : adjCoords) {
            if (nextPos.second < 0) continue;

            auto distanceIter     = positionDistances.find(nextPos.first);
            const int estDistance = currentDistance + nextPos.second;
            if (distanceIter == positionDistances.end() || estDistance < distanceIter->second) {
                prevPositionMap[nextPos.first]   = currentPos;
                positionDistances[nextPos.first] = estDistance;
                toVisit.emplace(estDistance + distCb(nextPos.first, destination), nextPos.first);
            }
        }
    }
    if (!endReached) return false;

    const std::size_t pathLength = positionDistances[destination];
    TNode currentPos             = destination;
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

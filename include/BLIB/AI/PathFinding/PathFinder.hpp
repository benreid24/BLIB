#ifndef BLIB_AI_PATHFINDING_PATHFINDER_HPP
#define BLIB_AI_PATHFINDING_PATHFINDER_HPP

#include <BLIB/AI/PathFinding/PathMap.hpp>
#include <BLIB/Entities/Entity.hpp>
#include <BLIB/Util/HashCombine.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <cmath>
#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace ai
{
namespace helpers
{
template<typename T>
struct SfVector2Hash {
    std::size_t operator()(const sf::Vector2<T>& value) const {
        std::hash<T> hasher;
        return bl::util::hashCombine(hasher(value.x), hasher(value.y));
    }
};

template<typename T>
struct SfVector3Hash {
    std::size_t operator()(const sf::Vector3<T>& value) const {
        std::hash<T> hasher;
        return bl::util::hashCombine(bl::util::hashCombine(hasher(value.x), hasher(value.y)),
                                     hasher(value.z));
    }
};

template<typename T>
int Vector2ManhattanDist(const sf::Vector2<T>& from, const sf::Vector2<T>& to, entity::Entity) {
    return std::abs(from.x - to.x) + std::abs(from.y - to.y);
}

template<typename T>
int Vector2RealDist(const sf::Vector2<T>& from, const sf::Vector2<T>& to, entity::Entity) {
    const int dx = from.x - to.x;
    const int dy = from.y - to.y;
    return std::sqrt(dx * dx + dy * dy);
}

template<typename T>
int Vector3ManhattanDist(const sf::Vector3<T>& from, const sf::Vector3<T>& to, entity::Entity) {
    return std::abs(from.x - to.x) + std::abs(from.y - to.y) + std::abs(from.z - to.z);
}

template<typename T>
int Vector3RealDist(const sf::Vector2<T>& from, const sf::Vector2<T>& to, entity::Entity) {
    const int dx = from.x - to.x;
    const int dy = from.y - to.y;
    const int dz = from.z - to.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
} // namespace helpers

template<typename TCoord, typename CoordHash>
class PathFinder {
public:
    using Heuristic =
        std::function<int(const TCoord& from, const TCoord& to, entity::Entity mover)>;

    PathFinder(Heuristic heuristic);

    void setHeuristic(Heuristic heuristic);

    bool findPath(const TCoord& start, const TCoord& destination, entity::Entity mover,
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
                                         entity::Entity mover, const PathMap<TCoord>& map,
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

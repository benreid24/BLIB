#include <BLIB/Tilemap/Direction.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace tmap
{
Direction oppositeDirection(Direction dir) {
    switch (dir) {
    case Direction::Up:
        return Direction::Down;
    case Direction::Right:
        return Direction::Left;
    case Direction::Down:
        return Direction::Up;
    case Direction::Left:
        return Direction::Right;
    default:
        BL_LOG_WARN << "Invalid direction: " << dir;
        return Direction::Up;
    }
}

Direction nextClockwiseDirection(Direction dir) {
    switch (dir) {
    case Direction::Up:
        return Direction::Right;
    case Direction::Right:
        return Direction::Down;
    case Direction::Down:
        return Direction::Left;
    case Direction::Left:
        return Direction::Up;
    default:
        BL_LOG_WARN << "Invalid direction: " << dir;
        return Direction::Up;
    }
}

Direction nextCounterClockwiseDirection(Direction dir) {
    switch (dir) {
    case Direction::Up:
        return Direction::Left;
    case Direction::Right:
        return Direction::Up;
    case Direction::Down:
        return Direction::Right;
    case Direction::Left:
        return Direction::Down;
    default:
        BL_LOG_WARN << "Invalid direction: " << dir;
        return Direction::Up;
    }
}

Direction directionFromString(const std::string& dir) {
    switch (dir.empty() ? 'u' : dir[0]) {
    case 'u':
        return Direction::Up;
    case 'r':
        return Direction::Right;
    case 'd':
        return Direction::Down;
    default:
        return Direction::Left;
    }
}

std::string directionToString(Direction dir) {
    switch (dir) {
    case Direction::Up:
        return "up";
    case Direction::Right:
        return "right";
    case Direction::Down:
        return "down";
    default:
        return "left";
    }
}

} // namespace tmap
} // namespace bl

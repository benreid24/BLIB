#ifndef BLIB_TILEMAP_DIRECTION_HPP
#define BLIB_TILEMAP_DIRECTION_HPP

#include <cstdint>
#include <string>

namespace bl
{
namespace tmap
{
/**
 * @brief Which direction an entity is facing in a tilemap
 *
 * @ingroup Tilemap
 */
enum struct Direction : std::uint8_t { Up = 0, Right = 1, Down = 2, Left = 3 };

/**
 * @brief Returns the Direction that is opposite of the given direction
 *
 * @param dir The direction to get the opposite of
 * @return Direction The direction facing opposite of dir
 *
 * @ingroup Tilemap
 */
Direction oppositeDirection(Direction dir);

/**
 * @brief Returns the next clockwise direction from the given current direction
 *
 * @param dir The direction to move clockwise from
 * @return Direction The next clockwise direction
 *
 * @ingroup Tilemap
 */
Direction nextClockwiseDirection(Direction dir);

/**
 * @brief Returns the next counterclockwise direction from the given current direction
 *
 * @param dir The direction to move counterclockwise from
 * @return Direction The next counterclockwise direction
 *
 * @ingroup Tilemap
 */
Direction nextCounterClockwiseDirection(Direction dir);

/**
 * @brief Converts the given string into a direction
 *
 * @param dir "up", "right", "down", "left"
 * @return Direction The corresponding direction
 *
 * @ingroup Tilemap
 */
Direction directionFromString(const std::string& dir);

/**
 * @brief Converts the given direction to a string
 *
 * @param dir The direction to convert
 * @return std::string The string representation of the direction
 *
 * @ingroup Tilemap
 */
std::string directionToString(Direction dir);

} // namespace tmap
} // namespace bl

#endif

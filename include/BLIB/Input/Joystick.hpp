#ifndef BLIB_INPUT_JOYSTICK_HPP
#define BLIB_INPUT_JOYSTICK_HPP

#include <SFML/Window/Joystick.hpp>

namespace bl
{
namespace input
{
/**
 * @brief Helper struct representing a full joystick for inputs
 *
 * @ingroup Input
 *
 */
struct Joystick {
    sf::Joystick::Axis verticalAxis;
    sf::Joystick::Axis horizontalAxis;

    /**
     * @brief Reads the joystick value into a vector with components in range [-100, 100]
     *
     */
    sf::Vector2f read() const;
};

} // namespace input
} // namespace bl

#endif

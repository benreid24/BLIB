#ifndef BLIB_INPUT_MOVEMENTCONTROL_HPP
#define BLIB_INPUT_MOVEMENTCONTROL_HPP

#include <BLIB/Input/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace bl
{
namespace input
{
/**
 * @brief Control data for a movement control (ie WASD or a joystick)
 *
 * @ingroup Input
 *
 */
struct MovementControl {
    /// @brief The type of input that drives the control
    enum struct Type { Joystick, Keyboard };

    /// @brief Key data if the keyboard drives the control
    struct KeyConfig {
        sf::Keyboard::Key up;
        sf::Keyboard::Key right;
        sf::Keyboard::Key down;
        sf::Keyboard::Key left;
    };

    union {
        Joystick joystick;
        KeyConfig keys;
    };

    /**
     * @brief Defaults to keyboard type with WASD
     *
     */
    MovementControl();
};

} // namespace input
} // namespace bl

#endif

#ifndef BLIB_INPUT_CONTROLLERS_HPP
#define BLIB_INPUT_CONTROLLERS_HPP

#include <SFML/Window/Joystick.hpp>

/**
 * @addtogroup Controllers
 * @ingroup Input
 * @brief Collection of constexpr configs for some common controllers (joysticks)
 *
 */

namespace bl
{
namespace input
{
/// @brief Collection of constexpr configs for some common controllers (joysticks)
namespace controllers
{
/**
 * @brief Some constansts for buttons and axes for an Xbox 360 controller. Tested on Windows. For
 *        the sticks left and up are negative. For the D-Pad left is negative but up is positive.
 *        For the triggers LT is positive and RT is negative
 *
 * @ingroup Input
 * @ingroup Controllers
 *
 */
struct Xbox360 {
    static constexpr float StickDeadZone           = 22.f;
    static constexpr float StickDeadZoneNormalized = StickDeadZone / 100.f;

    static constexpr unsigned int A     = 0;
    static constexpr unsigned int B     = 1;
    static constexpr unsigned int X     = 2;
    static constexpr unsigned int Y     = 3;
    static constexpr unsigned int RB    = 4;
    static constexpr unsigned int LB    = 5;
    static constexpr unsigned int Back  = 6;
    static constexpr unsigned int Start = 7;
    static constexpr unsigned int LS    = 8;
    static constexpr unsigned int RS    = 9;

    static constexpr sf::Joystick::Axis LSHorizontal = sf::Joystick::Axis::X;
    static constexpr sf::Joystick::Axis LSVertical   = sf::Joystick::Axis::Y;
    static constexpr sf::Joystick::Axis RSHorizontal = sf::Joystick::Axis::U;
    static constexpr sf::Joystick::Axis RSVertical   = sf::Joystick::Axis::V;

    static constexpr sf::Joystick::Axis DPadHorizontal = sf::Joystick::Axis::PovX;
    static constexpr sf::Joystick::Axis DPadVertical   = sf::Joystick::Axis::PovY;

    static constexpr sf::Joystick::Axis Triggers = sf::Joystick::Axis::Z;

    /**
     * @brief Returns whether or not the given controller is an Xbox 360 one or not. Tested on
     *        Windows only
     *
     * @param joystickId The joystick to test
     * @return True if it is an Xbox 360 controller, false otherwise
     */
    static bool is(unsigned int joystickId) {
        const sf::Joystick::Identification id = sf::Joystick::getIdentification(joystickId);
        return id.productId == 673 && id.vendorId == 1118;
    }
};

} // namespace controllers
} // namespace input
} // namespace bl

#endif

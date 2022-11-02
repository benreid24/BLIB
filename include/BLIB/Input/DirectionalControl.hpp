#ifndef BLIB_INPUT_DIRECTIONALCONTROL_HPP
#define BLIB_INPUT_DIRECTIONALCONTROL_HPP

#include <BLIB/Input/Joystick.hpp>

namespace bl
{
namespace input
{
/**
 * @brief Control data for a directional input
 *
 * @ingroup Input
 *
 */
struct DirectionalControl {
    /// @brief The type of input that drives the control
    enum struct Type { Mouse, Joystick };

    Type type;
    Joystick joystick;

    /**
     * @brief Defaults to mouse control
     *
     */
    DirectionalControl();
};

} // namespace input
} // namespace bl

#endif

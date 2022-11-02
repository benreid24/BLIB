#ifndef BLIB_INPUT_CONTROL_HPP
#define BLIB_INPUT_CONTROL_HPP

#include <BLIB/Input/DirectionalControl.hpp>
#include <BLIB/Input/DispatchPolicy.hpp>
#include <BLIB/Input/Joystick.hpp>
#include <BLIB/Input/MovementControl.hpp>
#include <BLIB/Input/Trigger.hpp>
#include <BLIB/Input/TriggerControl.hpp>

namespace bl
{
namespace input
{
/**
 * @brief Represents a control in the input system. A Control is a Trigger and a DispatchPolicy
 *        which fully define the control and its behavior
 *
 * @ingroup Input
 *
 */
struct Control {
    /// The type of control
    enum Type {
        /// @brief Controls such as buttons that trigger a single action
        SingleTrigger,

        /// @brief Composite directional control, such as a mouse or joystick
        Directional,

        /// @brief Composite movement such as WASD or a joystick
        Movement
    };

    /**
     * @brief Construct a new Control from it's trigger and policy
     *
     * @param policy
     * @param trigger
     */
    Control(DispatchPolicy policy, Trigger trigger);

    union {
        TriggerControl triggerControl;
        DirectionalControl directionalControl;
        MovementControl movementControl;
    };
};

} // namespace input
} // namespace bl

#endif

#ifndef BLIB_INPUT_CONTROL_HPP
#define BLIB_INPUT_CONTROL_HPP

#include <BLIB/Input/DirectionalControl.hpp>
#include <BLIB/Input/Joystick.hpp>
#include <BLIB/Input/MovementControl.hpp>
#include <BLIB/Input/Trigger.hpp>

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
     * @brief Construct a new Control of the given type
     *
     * @param type The type of control to create
     * @param joystickMode True to be a joystick control, false for keyboard and mouse
     */
    Control(Type type, bool joystickMode);

    /**
     * @brief Saves this control config to the engine configuration store
     *
     * @param prefix The prefix to use for keys used to save the config
     */
    void saveToConfig(const std::string& prefix) const;

    /**
     * @brief Loads this control config from the engine configuration store
     *
     * @param prefix The prefix to use for keys to laod from the config
     */
    void loadFromConfig(const std::string& prefix);

    Type type;
    union {
        Trigger triggerControl;
        DirectionalControl directionalControl;
        MovementControl movementControl;
    };
};

} // namespace input
} // namespace bl

#endif

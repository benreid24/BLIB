#ifndef BLIB_INPUT_MOVEMENTCONTROL_HPP
#define BLIB_INPUT_MOVEMENTCONTROL_HPP

#include <BLIB/Input/DispatchType.hpp>
#include <BLIB/Input/Joystick.hpp>
#include <BLIB/Input/Trigger.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <stack>
#include <vector>

namespace bl
{
namespace input
{
class Actor;

/**
 * @brief Control data for a movement control (ie WASD or a joystick)
 *
 * @ingroup Input
 *
 */
class MovementControl {
public:
    /// @brief The type of input that drives the control
    enum struct Type { Joystick, Triggers };

    /**
     * @brief Creates the movement control with the given mode
     *
     * @param joystickMode True to be a joystick control, false for keyboard and mouse
     *
     */
    MovementControl(bool joystickMode);

    /**
     * @brief Determines what direction the given event activated, if any
     *
     * @param event The event to process
     * @return DispatchType _INVALID if not a movement, otherwise the direction
     */
    DispatchType process(const sf::Event& event);

    /**
     * @brief Returns the type of movement control being used
     *
     * @return Type The type of movement control being used
     */
    Type controlType() const;

    /**
     * @brief Returns the up trigger control
     *
     */
    Trigger& upControl();

    /**
     * @brief Returns the up trigger control
     *
     */
    const Trigger& upControl() const;

    /**
     * @brief Returns the right trigger control
     *
     */
    Trigger& rightControl();

    /**
     * @brief Returns the right trigger control
     *
     */
    const Trigger& rightControl() const;

    /**
     * @brief Returns the down trigger control
     *
     */
    Trigger& downControl();

    /**
     * @brief Returns the down trigger control
     *
     */
    const Trigger& downControl() const;

    /**
     * @brief Returns the left trigger control
     *
     */
    Trigger& leftControl();

    /**
     * @brief Returns the left trigger control
     *
     */
    const Trigger& leftControl() const;

    /**
     * @brief Returns the joystick config for this control
     *
     */
    Joystick& joystickControl();

    /**
     * @brief Returns the joystick config for this control
     *
     */
    const Joystick& joystickControl() const;

    /**
     * @brief Returns a normlized vector indicating which direction is being moved in. up/forward is
     *        positive y, right is positive x
     *
     * @return sf::Vector2f The direction being moved in. Components in range [-1, 1]
     */
    sf::Vector2f readValue() const;

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

private:
    struct KeyConfig {
        Trigger up;
        Trigger right;
        Trigger down;
        Trigger left;

        KeyConfig(bool jm);
    };

    Type type;
    union {
        Joystick joystick;
        KeyConfig keys;
    };

    friend class Actor;
};

} // namespace input
} // namespace bl

#endif

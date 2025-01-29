#ifndef BLIB_INPUT_TRIGGER_HPP
#define BLIB_INPUT_TRIGGER_HPP

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstdint>
#include <string>

namespace bl
{
/// @brief Input system for building event based control systems
namespace input
{
/**
 * @brief Wrapper around keyboard and mouse buttons. Triggers activate Controls
 *
 * @ingroup Input
 *
 */
struct Trigger {
    /**
     * @brief The type of Trigger. Mouse wheel triggers never stay active and only fire events when
     *        they are first moved, unless the Trigger is in Toggle mode
     *
     */
    enum Type : std::uint8_t {
        Invalid          = 0,
        Key              = 1,
        MouseButton      = 2,
        MouseWheelUp     = 3,
        MouseWheelDown   = 4,
        JoystickButton   = 5,
        JoystickPositive = 6,
        JoystickNegative = 7
    };

    /**
     * @brief Creates an empty invalid Trigger
     *
     * @param joystickMode True to be a joystick control, false for keyboard and mouse
     *
     */
    Trigger(bool joystickMode);

    /**
     * @brief Updates to be triggerd by the given key
     *
     * @param key The key to be active on
     */
    void triggerOnKey(sf::Keyboard::Key key);

    /**
     * @brief Updates to be triggered by the given mouse button
     *
     * @param button The mouse button to be active on
     */
    void triggerOnMouseButton(sf::Mouse::Button button);

    /**
     * @brief Trigger for mouse wheel inputs.
     *
     * @param wheel The wheel to trigger on
     * @param upOrRight True to trigger on positive input, false for negative
     */
    void triggerOnMouseWheel(sf::Mouse::Wheel wheel, bool upOrRight);

    /**
     * @brief Triggers for the given joystick button
     *
     * @param joystickButton The button to trigger on
     */
    void triggerOnJoystickButton(unsigned int joystickButton);

    /**
     * @brief Creates a trigger for joystick stick positions
     *
     * @param axis The axis to trigger on
     * @param positive Whether to trigger on positive or negative stick input
     */
    void triggerOnJoystickAxis(sf::Joystick::Axis axis, bool positive);

    /**
     * @brief Returns whether or not this trigger is currently active
     *
     */
    bool active() const;

    /**
     * @brief Processes the given event to update the control state
     *
     * @param event The event to check
     * @return True if this control is now active, false otherwise
     */
    bool process(const sf::Event& event);

    /**
     * @brief Updates this trigger data from the given event
     *
     * @param event The event to update the Trigger from
     * @return True if the event was usable for setting this trigger, false otherwise
     */
    bool configureFromEvent(const sf::Event& event);

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

    /**
     * @brief Returns a string representation of this trigger for presenting to a user
     *
     */
    std::string toString() const;

    /**
     * @brief Returns the type of input that triggers this
     *
     */
    Type triggerType() const;

    /**
     * @brief Returns whether or not this input is toggled or needs to be held
     *
     */
    bool isToggle() const;

    /**
     * @brief Set whether or not this input is toggled or needs to be held
     *
     * @param toggle True to toggle, false to hold to stay active
     */
    void setIsToggle(bool toggle);

    /**
     * @brief Returns the key that triggers this input
     *
     */
    sf::Keyboard::Key keyTrigger() const;

    /**
     * @brief Returns the mouse button that triggers this input
     *
     */
    sf::Mouse::Button mouseButtonTrigger() const;

    /**
     * @brief Returns the mouse wheel that triggers this input
     *
     */
    sf::Mouse::Wheel mouseWheelTrigger() const;

    /**
     * @brief Returns the joystick button that triggers this input
     *
     */
    unsigned int joystickButtonTrigger() const;

    /**
     * @brief Returns the joystick axis that triggers this input
     *
     */
    sf::Joystick::Axis joystickAxisTrigger() const;

private:
    const bool joystickMode;
    Type type;
    bool toggle;
    union {
        sf::Keyboard::Key key;
        sf::Mouse::Button mouseButton;
        sf::Mouse::Wheel mouseWheel;
        unsigned int joystickButton;
        sf::Joystick::Axis joystickAxis;
    };
    bool nowActive;

    bool makeOrtoggleActive();
    void makeInactive(); // no effect if in toggle mode
};

} // namespace input
} // namespace bl

#endif

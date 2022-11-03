#ifndef BLIB_INPUT_TRIGGER_HPP
#define BLIB_INPUT_TRIGGER_HPP

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
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
     * @brief The type of Trigger
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
     */
    Trigger();

    /**
     * @brief Creates a Trigger for the given key
     *
     * @param key The key to be active on
     */
    Trigger(sf::Keyboard::Key key);

    /**
     * @brief Creates a Trigger for the given mouse button
     *
     * @param button The mouse button to be active on
     */
    Trigger(sf::Mouse::Button button);

    /**
     * @brief Creates a trigger for mouse wheel inputs
     *
     * @param type Either MouseWheelUp or MouseWheelDown
     * @param wheel The mouse wheel to trigger on
     */
    Trigger(Type type, sf::Mouse::Wheel wheel = sf::Mouse::Wheel::VerticalWheel);

    /**
     * @brief Creates a trigger for the given joystick button
     *
     * @param joystickButton The button to trigger on
     */
    Trigger(unsigned int joystickButton);

    /**
     * @brief Creates a trigger for joystick stick positions
     *
     * @param type Either JoystickPositive or JoystickNegative
     * @param axis The axis to trigger on
     */
    Trigger(Type type, sf::Joystick::Axis axis);

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
    bool process(const sf::Event& event) const;

    /**
     * @brief Updates this trigger data from the given event
     *
     * @param event The event to update the Trigger from
     * @return True if the event was usable for setting this trigger, false otherwise
     */
    bool updateFromEvent(const sf::Event& event);

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

    /// @brief Represents whether this trigger is a toggle or regular control
    enum Mode { Standard = 0, Toggle = 1 };

    Type type;
    Mode mode;
    union {
        sf::Keyboard::Key key;
        sf::Mouse::Button mouseButton;
        sf::Mouse::Wheel mouseWheel;
        unsigned int joystickButton;
        sf::Joystick::Axis joystickAxis;
    };

private:
    bool nowActive;

    void makeOrtoggleActive();
    void makeInactive(); // no effect if in toggle mode
};

} // namespace input
} // namespace bl

#endif

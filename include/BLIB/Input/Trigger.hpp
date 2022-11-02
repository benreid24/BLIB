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
    } type;

    /**
     * @brief Parses the Trigger from the given string
     *
     * @param str
     * @return Trigger
     */
    static Trigger fromString(const std::string& str);

    /**
     * @brief Creates a Trigger from the given event
     *
     * @param event The event to create the Trigger from
     * @return Trigger The produced Trigger. May be Invalid
     */
    static Trigger fromEvent(const sf::Event& event);

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
     * @brief Returns the string representation of the Trigger
     *
     */
    const std::string& toString() const;

    /**
     * @brief Returns whether or not this trigger is currently active
     *
     * @param joystickId The joystick to query if using joysticks
     *
     */
    bool active(unsigned int joystickId) const;

    /**
     * @brief Returns whether or not the given event triggers this control
     *
     * @param event The event to check
     * @return True if this control is triggered by the event, false otherwise
     */
    bool triggeredBy(const sf::Event& event) const;

    union {
        sf::Keyboard::Key key;
        sf::Mouse::Button mouseButton;
        sf::Mouse::Wheel mouseWheel;
        unsigned int joystickButton;
        sf::Joystick::Axis joystickAxis;
    };
};

} // namespace input
} // namespace bl

#endif

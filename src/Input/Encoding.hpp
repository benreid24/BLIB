#ifndef BLIB_INPUT_ENCODING_HPP
#define BLIB_INPUT_ENCODING_HPP

#include <SFML/Window.hpp>
#include <cstdint>
#include <string>

namespace bl
{
namespace input
{
struct Encoder {
    struct ControlInfo {
        enum Type {
            Invalid,
            Key,
            MouseButton,
            MouseWheelUp,
            MouseWheelDown,
            JoystickButton,
            JoystickAxis
        } type;

        union {
            sf::Keyboard::Key key;
            sf::Mouse::Button mouseButton;
            sf::Mouse::Wheel mouseWheel;
            unsigned int joystickButton;
            sf::Joystick::Axis joystickAxis;
        };

        ControlInfo();
        ControlInfo(sf::Keyboard::Key key);
        ControlInfo(sf::Mouse::Button mbut);
        ControlInfo(sf::Mouse::Wheel wheel, bool upOrRight);
        ControlInfo(unsigned int jbut);
        ControlInfo(sf::Joystick::Axis axis);
    };

    static std::string toString(const ControlInfo& control);

    static ControlInfo fromString(const std::string& encoded);
};

} // namespace input
} // namespace bl

#endif

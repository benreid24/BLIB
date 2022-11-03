#include <BLIB/Input/Trigger.hpp>

namespace bl
{
namespace input
{


Trigger::Trigger()
: type(Type::Invalid)
, mode(Mode::Standard)
, nowActive(false) {}

Trigger::Trigger(sf::Keyboard::Key key)
: type(Type::Key)
, mode(Mode::Standard)
, key(key)
, nowActive(false) {}

Trigger::Trigger(sf::Mouse::Button mbut)
: type(Type::MouseButton)
, mode(Mode::Standard)
, mouseButton(mbut)
, nowActive(false) {}

Trigger::Trigger(Type type, sf::Mouse::Wheel wheel)
: type(type)
, mode(Mode::Standard)
, mouseWheel(wheel)
, nowActive(false) {}

Trigger::Trigger(unsigned int jbut)
: type(Type::JoystickButton)
, mode(Mode::Standard)
, joystickButton(jbut)
, nowActive(false) {}

Trigger::Trigger(Type type, sf::Joystick::Axis axis)
: type(type)
, mode(Mode::Standard)
, joystickAxis(axis)
, nowActive(false) {}

} // namespace input
} // namespace bl

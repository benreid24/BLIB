#include <BLIB/Input/Joystick.hpp>

#include <BLIB/Math/Vector.hpp>
#include <Input/Encoder.hpp>

namespace bl
{
namespace input
{
sf::Vector2f Joystick::read(unsigned int joystick) const {
    const float x = sf::Joystick::getAxisPosition(joystick, horizontalAxis);
    const float y = sf::Joystick::getAxisPosition(joystick, verticalAxis);
    return math::normalized(sf::Vector2f{x, y});
}

bool Joystick::process(const sf::Event& event) {
    if (event.type == sf::Event::JoystickMoved) {
        if (event.joystickMove.axis == horizontalAxis) {
            cachedPosition.x = event.joystickMove.position;
            return true;
        }
        else if (event.joystickMove.axis == verticalAxis) {
            cachedPosition.y = event.joystickMove.position;
            return true;
        }
    }
    return false;
}

std::string Joystick::toString() const {
    return Joystick::axisToString(horizontalAxis) + " / " + Joystick::axisToString(verticalAxis);
}

std::string Joystick::axisToString(sf::Joystick::Axis a) {
    return Encoder::toString(Encoder::ControlInfo(a));
}

} // namespace input
} // namespace bl

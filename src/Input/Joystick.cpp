#include <BLIB/Input/Joystick.hpp>

#include <BLIB/Math/Vector.hpp>

namespace bl
{
namespace input
{
sf::Vector2f Joystick::read(unsigned int joystick) const {
    const float x = sf::Joystick::getAxisPosition(joystick, horizontalAxis);
    const float y = sf::Joystick::getAxisPosition(joystick, verticalAxis);
    return math::normalized(sf::Vector2f{x, y});
}

} // namespace input
} // namespace bl

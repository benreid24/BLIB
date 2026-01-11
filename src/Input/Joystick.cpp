#include <BLIB/Input/Joystick.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Math/Vector.hpp>
#include <Input/Encoder.hpp>

namespace bl
{
namespace input
{
namespace
{
std::string horKey(const std::string& prefix) { return prefix + ".horizontal.axis"; }
std::string horInvKey(const std::string& prefix) { return prefix + ".horizontal.inverted"; }
std::string vertKey(const std::string& prefix) { return prefix + ".vertical.axis"; }
std::string vertInvKey(const std::string& prefix) { return prefix + ".vertical.inverted"; }
} // namespace

Joystick::Joystick()
: verticalAxis(sf::Joystick::Axis::Y)
, horizontalAxis(sf::Joystick::Axis::X)
, verticalInverted(false)
, horizontalInverted(false) {}

sf::Vector2f Joystick::read(unsigned int joystick) const {
    float x = sf::Joystick::getAxisPosition(joystick, horizontalAxis);
    float y = sf::Joystick::getAxisPosition(joystick, verticalAxis);
    if (horizontalInverted) x = -x;
    if (verticalInverted) y = -y;
    return math::normalized(sf::Vector2f{x, y});
}

bool Joystick::process(const sf::Event& event) {
    const sf::Event::JoystickMoved* jm = event.getIf<sf::Event::JoystickMoved>();
    if (jm) {
        if (jm->axis == horizontalAxis) {
            cachedPosition.x = jm->position;
            if (horizontalInverted) cachedPosition.x = -cachedPosition.x;
            return true;
        }
        else if (jm->axis == verticalAxis) {
            cachedPosition.y = jm->position;
            if (verticalInverted) cachedPosition.y = -cachedPosition.y;
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

void Joystick::saveToConfig(const std::string& prefix) const {
    bl::engine::Configuration::set<std::string>(horKey(prefix), axisToString(horizontalAxis));
    bl::engine::Configuration::set<bool>(horInvKey(prefix), horizontalInverted);
    bl::engine::Configuration::set<std::string>(vertKey(prefix), axisToString(verticalAxis));
    bl::engine::Configuration::set<bool>(vertInvKey(prefix), verticalInverted);
}

void Joystick::loadFromConfig(const std::string& prefix) {
    const std::string hc = bl::engine::Configuration::getOrDefault<std::string>(horKey(prefix), "");
    if (!hc.empty()) {
        const Encoder::ControlInfo c = Encoder::fromString(hc);
        if (c.type == Encoder::ControlInfo::JoystickAxis) { horizontalAxis = c.joystickAxis; }
        horizontalInverted =
            bl::engine::Configuration::getOrDefault<bool>(horInvKey(prefix), horizontalInverted);
    }

    const std::string vc =
        bl::engine::Configuration::getOrDefault<std::string>(vertKey(prefix), "");
    if (!vc.empty()) {
        const Encoder::ControlInfo c = Encoder::fromString(vc);
        if (c.type == Encoder::ControlInfo::JoystickAxis) { verticalAxis = c.joystickAxis; }
        verticalInverted =
            bl::engine::Configuration::getOrDefault<bool>(vertInvKey(prefix), verticalInverted);
    }
}

} // namespace input
} // namespace bl

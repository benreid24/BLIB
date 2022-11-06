#include <BLIB/Input/DirectionalControl.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Input/InputSystem.hpp>
#include <BLIB/Math/Vector.hpp>
#include <Input/Encoder.hpp>

namespace bl
{
namespace input
{
namespace
{
const std::string MouseStr = "Mouse";
const std::string StickStr = "Joystick";

std::string typeKey(const std::string& prefix) { return prefix + ".type"; }
std::string horKey(const std::string& prefix) { return prefix + ".horizontal"; }
std::string vertKey(const std::string& prefix) { return prefix + ".vertical"; }
} // namespace

DirectionalControl::DirectionalControl(bool jm)
: type(jm ? Type::Joystick : Type::Mouse) {}

bool DirectionalControl::process(const InputSystem& system, const sf::Event& event) {
    if (type == Type::Mouse && event.type == sf::Event::MouseMoved) {
        normalizedDirection = system.mouseUnitVector();
        return true;
    }
    else if (type == Type::Joystick && joystick.process(event)) {
        normalizedDirection = math::normalized(joystick.cachedPosition);
        return true;
    }
    return false;
}

void DirectionalControl::saveToConfig(const std::string& prefix) const {
    bl::engine::Configuration::set<std::string>(typeKey(prefix),
                                                type == Type::Mouse ? MouseStr : StickStr);
    if (type == Type::Joystick) {
        bl::engine::Configuration::set<std::string>(
            horKey(prefix), Encoder::toString(Encoder::ControlInfo(joystick.horizontalAxis)));
        bl::engine::Configuration::set<std::string>(
            vertKey(prefix), Encoder::toString(Encoder::ControlInfo(joystick.verticalAxis)));
    }
}

void DirectionalControl::loadFromConfig(const std::string& prefix) {
    type = bl::engine::Configuration::getOrDefault<std::string>(typeKey(prefix), MouseStr) ==
                   MouseStr ?
               Type::Mouse :
               Type::Joystick;
    if (type == Type::Joystick) {
        joystick.horizontalAxis =
            Encoder::fromString(bl::engine::Configuration::get<std::string>(horKey(prefix)))
                .joystickAxis;
        joystick.verticalAxis =
            Encoder::fromString(bl::engine::Configuration::get<std::string>(vertKey(prefix)))
                .joystickAxis;
    }
}

} // namespace input
} // namespace bl

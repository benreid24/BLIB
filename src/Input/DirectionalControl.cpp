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
std::string stickKey(const std::string& prefix) { return prefix + ".joystick"; }
} // namespace

DirectionalControl::DirectionalControl(bool jm)
: type(jm ? Type::Joystick : Type::Mouse) {}

bool DirectionalControl::process(const InputSystem& system, const sf::Event& event) {
    if (type == Type::Mouse && event.is<sf::Event::MouseMoved>()) {
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
    if (type == Type::Joystick) { joystick.saveToConfig(stickKey(prefix)); }
}

void DirectionalControl::loadFromConfig(const std::string& prefix) {
    type = bl::engine::Configuration::getOrDefault<std::string>(typeKey(prefix), MouseStr) ==
                   MouseStr ?
               Type::Mouse :
               Type::Joystick;
    if (type == Type::Joystick) { joystick.loadFromConfig(stickKey(prefix)); }
}

std::string DirectionalControl::toString() const {
    if (type == Type::Joystick) { return joystick.toString(); }
    else { return "Mouse"; }
}

} // namespace input
} // namespace bl

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
: config{Config{.type = jm ? Type::Joystick : Type::Mouse}} {}

bool DirectionalControl::process(const InputSystem& system, const sf::Event& event) {
    if (config.type == Type::Mouse && event.is<sf::Event::MouseMoved>()) {
        normalizedDirection   = system.mouseUnitVector();
        normalizedDirection.y = -normalizedDirection.y;
        return true;
    }
    else if (config.type == Type::Joystick && config.joystick.process(event)) {
        normalizedDirection = glm::normalize(config.joystick.cachedPosition);
        return true;
    }
    return false;
}

void DirectionalControl::saveToConfig(const std::string& prefix) const {
    bl::engine::Configuration::set<std::string>(typeKey(prefix),
                                                config.type == Type::Mouse ? MouseStr : StickStr);
    if (config.type == Type::Joystick) { config.joystick.saveToConfig(stickKey(prefix)); }
}

void DirectionalControl::loadFromConfig(const std::string& prefix) {
    config.type =
        bl::engine::Configuration::getOrDefault<std::string>(
            typeKey(prefix), config.type == Type::Mouse ? MouseStr : StickStr) == MouseStr ?
            Type::Mouse :
            Type::Joystick;
    if (config.type == Type::Joystick) { config.joystick.loadFromConfig(stickKey(prefix)); }
}

std::string DirectionalControl::toString() const {
    if (config.type == Type::Joystick) { return config.joystick.toString(); }
    else { return "Mouse"; }
}

} // namespace input
} // namespace bl

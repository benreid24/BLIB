#include <BLIB/Input/MovementControl.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Math/Vector.hpp>
#include <Input/Consts.hpp>
#include <Input/Encoder.hpp>

namespace bl
{
namespace input
{
namespace
{
const std::string TriggerStr  = "Triggers";
const std::string JoystickStr = "Joystick";

std::string upKey(const std::string& prefix) { return prefix + ".up"; }
std::string rightKey(const std::string& prefix) { return prefix + ".right"; }
std::string downKey(const std::string& prefix) { return prefix + ".down"; }
std::string leftKey(const std::string& prefix) { return prefix + ".left"; }
std::string typeKey(const std::string& prefix) { return prefix + ".type"; }
std::string stickKey(const std::string& prefix) { return prefix + ".joystick"; }
} // namespace

MovementControl::MovementControl(bool jm)
: type(jm ? Type::Joystick : Type::Triggers) {
    if (jm) { new (&joystick) Joystick(); }
    else { new (&keys) KeyConfig(jm); }
}

void MovementControl::configure(const Config& config) {
    if (type != config.type) {
        BL_LOG_ERROR << "Type mismatch when configuring MovementControl";
        return;
    }
    if (type == Type::Triggers) {
        keys.up.triggerOnConfig(config.triggers.up);
        keys.right.triggerOnConfig(config.triggers.right);
        keys.down.triggerOnConfig(config.triggers.down);
        keys.left.triggerOnConfig(config.triggers.left);
    }
    else { joystick = config.joystick; }
}

DispatchType MovementControl::process(const sf::Event& event) {
    if (type == Type::Triggers) {
        if (keys.up.process(event)) return DispatchType::MovementUp;
        if (keys.right.process(event)) return DispatchType::MovementRight;
        if (keys.down.process(event)) return DispatchType::MovementDown;
        if (keys.left.process(event)) return DispatchType::MovementLeft;
    }
    else if (type == Type::Joystick) {
        const glm::vec2 old = joystick.cachedPosition;

        if (joystick.process(event)) {
            const sf::Event::JoystickMoved& jm = *event.getIf<sf::Event::JoystickMoved>();
            if (jm.axis == joystick.verticalAxis) {
                if (joystick.cachedPosition.y >= StickThreshold && old.y < StickThreshold)
                    return DispatchType::MovementUp;
                if (joystick.cachedPosition.y <= -StickThreshold && old.y > -StickThreshold)
                    return DispatchType::MovementDown;
            }
            else {
                if (joystick.cachedPosition.x >= StickThreshold && old.x < StickThreshold)
                    return DispatchType::MovementRight;
                if (joystick.cachedPosition.x <= -StickThreshold && old.y > -StickThreshold)
                    return DispatchType::MovementLeft;
            }
        }
    }
    return DispatchType::_INVALID;
}

MovementControl::Type MovementControl::controlType() const { return type; }

Trigger& MovementControl::upControl() { return keys.up; }

Trigger& MovementControl::rightControl() { return keys.right; }

Trigger& MovementControl::downControl() { return keys.down; }

Trigger& MovementControl::leftControl() { return keys.left; }

const Trigger& MovementControl::upControl() const { return keys.up; }

const Trigger& MovementControl::rightControl() const { return keys.right; }

const Trigger& MovementControl::downControl() const { return keys.down; }

const Trigger& MovementControl::leftControl() const { return keys.left; }

Joystick& MovementControl::joystickControl() { return joystick; }

const Joystick& MovementControl::joystickControl() const { return joystick; }

glm::vec2 MovementControl::readValue() const {
    if (type == Type::Triggers) {
        glm::vec2 dir;
        if (keys.up.active()) dir.y += 1.f;
        if (keys.right.active()) dir.x += 1.f;
        if (keys.down.active()) dir.y -= 1.f;
        if (keys.left.active()) dir.x -= 1.f;
        return glm::normalize(dir);
    }
    else { return glm::normalize(joystick.cachedPosition); }
}

void MovementControl::saveToConfig(const std::string& prefix) const {
    using Config = bl::engine::Configuration;

    Config::set<std::string>(typeKey(prefix), type == Type::Triggers ? TriggerStr : JoystickStr);
    if (type == Type::Triggers) {
        keys.up.saveToConfig(upKey(prefix));
        keys.right.saveToConfig(rightKey(prefix));
        keys.down.saveToConfig(downKey(prefix));
        keys.left.saveToConfig(leftKey(prefix));
    }
    else { joystick.saveToConfig(stickKey(prefix)); }
}

void MovementControl::loadFromConfig(const std::string& prefix) {
    using Config = bl::engine::Configuration;

    type = Config::getOrDefault<std::string>(typeKey(prefix), TriggerStr) == TriggerStr ?
               Type::Triggers :
               Type::Joystick;
    if (type == Type::Triggers) {
        keys.up.loadFromConfig(upKey(prefix));
        keys.right.loadFromConfig(rightKey(prefix));
        keys.down.loadFromConfig(downKey(prefix));
        keys.left.loadFromConfig(leftKey(prefix));
    }
    else { joystick.loadFromConfig(stickKey(prefix)); }
}

std::string MovementControl::toString() const {
    if (type == Type::Triggers) {
        return "Up: " + keys.up.toString() + "\nRight: " + keys.right.toString() +
               "\nDown: " + keys.down.toString() + "\nLeft: " + keys.left.toString();
    }
    else { return joystick.toString(); }
}

MovementControl::KeyConfig::KeyConfig(bool jm)
: up(jm)
, right(jm)
, down(jm)
, left(jm) {}

} // namespace input
} // namespace bl

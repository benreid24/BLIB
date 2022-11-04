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
std::string horKey(const std::string& prefix) { return prefix + ".horizontal"; }
std::string vertKey(const std::string& prefix) { return prefix + ".vertical"; }
} // namespace

MovementControl::MovementControl()
: MovementControl(Trigger(sf::Keyboard::W), Trigger(sf::Keyboard::D), Trigger(sf::Keyboard::S),
                  Trigger(sf::Keyboard::A)) {}

MovementControl::MovementControl(const Trigger& up, const Trigger& right, const Trigger& down,
                                 const Trigger& left)
: type(Type::Triggers) {
    new (&keys) KeyConfig();
    keys.up    = up;
    keys.right = right;
    keys.down  = down;
    keys.left  = left;
}

MovementControl::MovementControl(sf::Joystick::Axis hor, sf::Joystick::Axis vert)
: type(Type::Joystick) {
    new (&joystick) Joystick();
    joystick.horizontalAxis = hor;
    joystick.verticalAxis   = vert;
}

DispatchType MovementControl::process(const sf::Event& event) {
    if (type == Type::Triggers) {
        if (keys.up.process(event)) return DispatchType::MovementUp;
        if (keys.right.process(event)) return DispatchType::MovementRight;
        if (keys.down.process(event)) return DispatchType::MovementDown;
        if (keys.left.process(event)) return DispatchType::MovementLeft;
    }
    else if (type == Type::Joystick) {
        if (joystick.process(event)) {
            if (joystick.cachedPosition.y >= StickThreshold) return DispatchType::MovementUp;
            if (joystick.cachedPosition.y <= StickThreshold) return DispatchType::MovementDown;
            if (joystick.cachedPosition.x >= StickThreshold) return DispatchType::MovementRight;
            if (joystick.cachedPosition.x <= StickThreshold) return DispatchType::MovementLeft;
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

sf::Vector2f MovementControl::readValue() const {
    if (type == Type::Triggers) {
        sf::Vector2f dir;
        if (keys.up.active()) dir.y += 1.f;
        if (keys.right.active()) dir.x += 1.f;
        if (keys.down.active()) dir.y -= 1.f;
        if (keys.left.active()) dir.x -= 1.f;
        return math::normalized(dir);
    }
    else {
        return math::normalized(joystick.cachedPosition);
    }
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
    else {
        Config::set<std::string>(horKey(prefix),
                                 Encoder::toString(Encoder::ControlInfo(joystick.horizontalAxis)));
        Config::set<std::string>(vertKey(prefix),
                                 Encoder::toString(Encoder::ControlInfo(joystick.verticalAxis)));
    }
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
    else {
        joystick.horizontalAxis =
            Encoder::fromString(Config::get<std::string>(horKey(prefix))).joystickAxis;
        joystick.verticalAxis =
            Encoder::fromString(Config::get<std::string>(vertKey(prefix))).joystickAxis;
    }
}

} // namespace input
} // namespace bl

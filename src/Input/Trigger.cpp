#include <BLIB/Input/Trigger.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <Input/Consts.hpp>
#include <Input/Encoder.hpp>

namespace bl
{
namespace input
{
namespace
{
std::string toggleKey(const std::string& prefix) { return prefix + ".toggle"; }

std::string inputKey(const std::string& prefix) { return prefix + ".trigger"; }
} // namespace

Trigger::Trigger()
: type(Type::Invalid)
, toggle(false)
, nowActive(false) {}

Trigger::Trigger(sf::Keyboard::Key key)
: type(Type::Key)
, toggle(false)
, key(key)
, nowActive(false) {}

Trigger::Trigger(sf::Mouse::Button mbut)
: type(Type::MouseButton)
, toggle(false)
, mouseButton(mbut)
, nowActive(false) {}

Trigger::Trigger(sf::Mouse::Wheel wheel, bool up)
: type(up ? Type::MouseWheelUp : Type::MouseWheelDown)
, toggle(false)
, mouseWheel(wheel)
, nowActive(false) {}

Trigger::Trigger(unsigned int jbut)
: type(Type::JoystickButton)
, toggle(false)
, joystickButton(jbut)
, nowActive(false) {}

Trigger::Trigger(sf::Joystick::Axis axis, bool pos)
: type(pos ? Type::JoystickPositive : Type::JoystickNegative)
, toggle(false)
, joystickAxis(axis)
, nowActive(false) {}

bool Trigger::active() const { return nowActive; }

bool Trigger::process(const sf::Event& event) {
    switch (type) {
    case Type::Key:
        if (event.key.code == key) {
            if (event.type == sf::Event::KeyPressed) { return makeOrtoggleActive(); }
            else if (event.type == sf::Event::KeyReleased) {
                makeInactive();
            }
        }
        return false;

    case Type::MouseButton:
        if (event.mouseButton.button == mouseButton) {
            if (event.type == sf::Event::MouseButtonPressed) { return makeOrtoggleActive(); }
            else if (event.type == sf::Event::MouseButtonReleased) {
                makeInactive();
            }
        }
        return false;

    case Type::MouseWheelUp:
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == mouseWheel && event.mouseWheelScroll.delta > 0.f) {
                if (toggle) {
                    nowActive = !nowActive;
                    return nowActive;
                }
                return true;
            }
        }
        return false;

    case Type::MouseWheelDown:
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == mouseWheel && event.mouseWheelScroll.delta < 0.f) {
                if (toggle) {
                    nowActive = !nowActive;
                    return nowActive;
                }
                return true;
            }
        }
        return false;

    case Type::JoystickButton:
        if (event.joystickButton.button == joystickButton) {
            if (event.type == sf::Event::JoystickButtonPressed) { return makeOrtoggleActive(); }
            else if (event.type == sf::Event::JoystickButtonReleased) {
                makeInactive();
            }
        }
        return false;

    case Type::JoystickPositive:
        if (event.type == sf::Event::JoystickMoved) {
            if (event.joystickMove.axis == joystickAxis) {
                if (event.joystickMove.position >= StickThreshold) { return makeOrtoggleActive(); }
                makeInactive();
            }
        }
        return false;

    case Type::JoystickNegative:
        if (event.type == sf::Event::JoystickMoved) {
            if (event.joystickMove.axis == joystickAxis) {
                if (event.joystickMove.position <= StickThreshold) { return makeOrtoggleActive(); }
                makeInactive();
            }
        }
        return false;

    case Type::Invalid:
    default:
        return false;
    }
}

bool Trigger::configureFromEvent(const sf::Event& event) {
    switch (event.type) {
    case sf::Event::KeyPressed:
        type = Type::Key;
        key  = event.key.code;
        return true;

    case sf::Event::MouseButtonPressed:
        type        = Type::MouseButton;
        mouseButton = event.mouseButton.button;
        return true;

    case sf::Event::MouseWheelScrolled:
        type       = event.mouseWheelScroll.delta > 0.f ? Type::MouseWheelUp : Type::MouseWheelDown;
        mouseWheel = event.mouseWheelScroll.wheel;
        return true;

    case sf::Event::JoystickButtonPressed:
        type           = Type::JoystickButton;
        joystickButton = event.joystickButton.button;
        return true;

    case sf::Event::JoystickMoved:
        if (event.joystickMove.position >= StickThreshold) {
            type         = Type::JoystickPositive;
            joystickAxis = event.joystickMove.axis;
            return true;
        }
        else if (event.joystickMove.position <= StickThreshold) {
            type         = Type::JoystickNegative;
            joystickAxis = event.joystickMove.axis;
            return true;
        }
        return false;

    default:
        return false;
    }
}

void Trigger::saveToConfig(const std::string& prefix) const {
    bl::engine::Configuration::set<bool>(toggleKey(prefix), toggle);
    bl::engine::Configuration::set<std::string>(inputKey(prefix), toString());
}

void Trigger::loadFromConfig(const std::string& prefix) {
    using T = Encoder::ControlInfo::Type;

    const std::string str =
        bl::engine::Configuration::getOrDefault<std::string>(inputKey(prefix), "");
    if (str.empty()) return; // don't clobber default state if no config
    const Encoder::ControlInfo ctrl = Encoder::fromString(str);
    toggle = bl::engine::Configuration::getOrDefault<bool>(toggleKey(prefix));

    switch (ctrl.type) {
    case T::Key:
        type = Type::Key;
        key  = ctrl.key;
        break;

    case T::MouseButton:
        type        = Type::MouseButton;
        mouseButton = ctrl.mouseButton;
        break;

    case T::MouseWheelUp:
        type       = Type::MouseWheelUp;
        mouseWheel = ctrl.mouseWheel;
        break;

    case T::MouseWheelDown:
        type       = Type::MouseWheelDown;
        mouseWheel = ctrl.mouseWheel;
        break;

    case T::JoystickButton:
        type           = Type::JoystickButton;
        joystickButton = ctrl.joystickButton;
        break;

    case T::JoystickAxisPositive:
        type         = Type::JoystickPositive;
        joystickAxis = ctrl.joystickAxis;
        break;

    case T::JoystickAxisNegative:
        type         = Type::JoystickNegative;
        joystickAxis = ctrl.joystickAxis;
        break;

    default:
        type = Type::Invalid;
        break;
    }
}

std::string Trigger::toString() const {
    Encoder::ControlInfo ctrl;
    switch (type) {
    case Type::Key:
        ctrl = Encoder::ControlInfo(key);
        break;
    case Type::MouseButton:
        ctrl = Encoder::ControlInfo(mouseButton);
        break;
    case Type::MouseWheelUp:
        ctrl = Encoder::ControlInfo(mouseWheel, true);
        break;
    case Type::MouseWheelDown:
        ctrl = Encoder::ControlInfo(mouseWheel, false);
        break;
    case Type::JoystickButton:
        ctrl = Encoder::ControlInfo(joystickButton);
        break;
    case Type::JoystickPositive:
        ctrl = Encoder::ControlInfo(joystickAxis, true);
        break;
    case Type::JoystickNegative:
        ctrl = Encoder::ControlInfo(joystickAxis, false);
        break;
    default:
        break;
    }

    return Encoder::toString(ctrl);
}

Trigger::Type Trigger::triggerType() const { return type; }

bool Trigger::isToggle() const { return toggle; }

void Trigger::setIsToggle(bool t) { toggle = t; }

sf::Keyboard::Key Trigger::keyTrigger() const { return key; }

sf::Mouse::Button Trigger::mouseButtonTrigger() const { return mouseButton; }

sf::Mouse::Wheel Trigger::mouseWheelTrigger() const { return mouseWheel; }

unsigned int Trigger::joystickButtonTrigger() const { return joystickButton; }

sf::Joystick::Axis Trigger::joystickAxisTrigger() const { return joystickAxis; }

bool Trigger::makeOrtoggleActive() {
    const bool wasActive = nowActive;
    if (toggle) { nowActive = !nowActive; }
    else {
        nowActive = true;
    }
    return nowActive && !wasActive;
}

void Trigger::makeInactive() {
    if (!toggle) { nowActive = false; }
}

} // namespace input
} // namespace bl

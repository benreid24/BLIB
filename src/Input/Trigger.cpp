#include <BLIB/Input/Trigger.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Util/Visitor.hpp>
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

Trigger::Trigger(bool jm)
: joystickMode(jm)
, type(Type::Invalid)
, toggle(false)
, nowActive(false) {}

void Trigger::triggerOnKey(sf::Keyboard::Key k) {
    if (joystickMode) {
        BL_LOG_ERROR << "Trying to set Key trigger on joystick control";
        return;
    }
    key  = k;
    type = Type::Key;
}

void Trigger::triggerOnMouseButton(sf::Mouse::Button mb) {
    if (joystickMode) {
        BL_LOG_ERROR << "Trying to set MouseButton trigger on joystick control";
        return;
    }
    mouseButton = mb;
    type        = Type::MouseButton;
}

void Trigger::triggerOnMouseWheel(sf::Mouse::Wheel w, bool up) {
    if (joystickMode) {
        BL_LOG_ERROR << "Trying to set MouseWheel trigger on joystick control";
        return;
    }
    mouseWheel = w;
    type       = up ? Type::MouseWheelUp : Type::MouseWheelDown;
}

void Trigger::triggerOnJoystickButton(unsigned int but) {
    if (!joystickMode) {
        BL_LOG_ERROR << "Trying to set JoystickButton trigger on keyboard+mouse control";
        return;
    }
    joystickButton = but;
    type           = Type::JoystickButton;
}

void Trigger::triggerOnJoystickAxis(sf::Joystick::Axis a, bool pos) {
    if (!joystickMode) {
        BL_LOG_ERROR << "Trying to set JoystickAxis trigger on keyboard+mouse control";
        return;
    }
    joystickAxis = a;
    type         = pos ? Type::JoystickPositive : Type::JoystickNegative;
}

bool Trigger::active() const { return nowActive; }

bool Trigger::process(const sf::Event& event) {
    bool result = false;
    event.visit(util::Visitor{
        [this, &result](const sf::Event::KeyPressed& keyEvent) {
            if (type == Type::Key && keyEvent.code == key) {
                result = makeOrtoggleActive();
            }
        },
        [this](const sf::Event::KeyReleased& keyEvent) {
            if (type == Type::Key && keyEvent.code == key) {
                makeInactive();
            }
        },
        [this, &result](const sf::Event::MouseButtonPressed& mbEvent) {
            if (type == Type::MouseButton && mbEvent.button == mouseButton) {
                result = makeOrtoggleActive();
            }
        },
        [this](const sf::Event::MouseButtonReleased& mbEvent) {
            if (type == Type::MouseButton && mbEvent.button == mouseButton) {
                makeInactive();
            }
        },
        [this, &result](const sf::Event::MouseWheelScrolled& mwEvent) {
            if (mwEvent.wheel == mouseWheel) {
                if (type == Type::MouseWheelUp && mwEvent.delta > 0.f) {
                    if (toggle) {
                        nowActive = !nowActive;
                        result    = nowActive;
                    }
                    else {
                        result = true;
                    }
                }
                else if (type == Type::MouseWheelDown && mwEvent.delta < 0.f) {
                    if (toggle) {
                        nowActive = !nowActive;
                        result    = nowActive;
                    }
                    else {
                        result = true;
                    }
                }
            }
        },
        [this, &result](const sf::Event::JoystickButtonPressed& jbEvent) {
            if (type == Type::JoystickButton && jbEvent.button == joystickButton) {
                result = makeOrtoggleActive();
            }
        },
        [this](const sf::Event::JoystickButtonReleased& jbEvent) {
            if (type == Type::JoystickButton && jbEvent.button == joystickButton) {
                makeInactive();
            }
        },
        [this, &result](const sf::Event::JoystickMoved& jmEvent) {
            if (jmEvent.axis == joystickAxis) {
                if (type == Type::JoystickPositive && jmEvent.position >= StickThreshold) {
                    result = makeOrtoggleActive();
                }
                else if (type == Type::JoystickNegative && jmEvent.position <= -StickThreshold) {
                    result = makeOrtoggleActive();
                }
                else {
                    makeInactive();
                }
            }
        },
        [](const auto&) {
            // Handle any other event types
        }});
    return result;
}

bool Trigger::configureFromEvent(const sf::Event& event) {
    bool result = false;
    event.visit(util::Visitor{
        [this, &result](const sf::Event::KeyPressed& keyEvent) {
            if (!joystickMode) {
                type   = Type::Key;
                key    = keyEvent.code;
                result = true;
            }
        },
        [this, &result](const sf::Event::MouseButtonPressed& mbEvent) {
            if (!joystickMode) {
                type        = Type::MouseButton;
                mouseButton = mbEvent.button;
                result      = true;
            }
        },
        [this, &result](const sf::Event::MouseWheelScrolled& mwEvent) {
            if (!joystickMode) {
                type       = mwEvent.delta > 0.f ? Type::MouseWheelUp : Type::MouseWheelDown;
                mouseWheel = mwEvent.wheel;
                result     = true;
            }
        },
        [this, &result](const sf::Event::JoystickButtonPressed& jbEvent) {
            if (joystickMode) {
                type           = Type::JoystickButton;
                joystickButton = jbEvent.button;
                result         = true;
            }
        },
        [this, &result](const sf::Event::JoystickMoved& jmEvent) {
            if (joystickMode) {
                if (jmEvent.position >= StickThreshold) {
                    type         = Type::JoystickPositive;
                    joystickAxis = jmEvent.axis;
                    result       = true;
                }
                else if (jmEvent.position <= -StickThreshold) {
                    type         = Type::JoystickNegative;
                    joystickAxis = jmEvent.axis;
                    result       = true;
                }
            }
        },
        [](const auto&) {
            // Handle any other event types
        }});
    return result;
}

void Trigger::saveToConfig(const std::string& prefix) const {
    if (type == Type::Invalid) {
        BL_LOG_WARN << "Refusing to save unset control: " << prefix;
        return;
    }
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
    case Type::Invalid:
    default:
        return "UNSET";
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

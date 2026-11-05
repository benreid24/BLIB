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
, config{Config{.type = Type::Invalid, .toggle = false, .key = sf::Keyboard::Key::Unknown}}
, nowActive(false) {}

void Trigger::triggerOnKey(sf::Keyboard::Key k) {
    if (joystickMode) {
        BL_LOG_ERROR << "Trying to set Key trigger on joystick control";
        return;
    }
    config.key  = k;
    config.type = Type::Key;
}

void Trigger::triggerOnMouseButton(sf::Mouse::Button mb) {
    if (joystickMode) {
        BL_LOG_ERROR << "Trying to set MouseButton trigger on joystick control";
        return;
    }
    config.mouseButton = mb;
    config.type        = Type::MouseButton;
}

void Trigger::triggerOnMouseWheel(sf::Mouse::Wheel w, bool up) {
    if (joystickMode) {
        BL_LOG_ERROR << "Trying to set MouseWheel trigger on joystick control";
        return;
    }
    config.mouseWheel = w;
    config.type       = up ? Type::MouseWheelUp : Type::MouseWheelDown;
}

void Trigger::triggerOnJoystickButton(unsigned int but) {
    if (!joystickMode) {
        BL_LOG_ERROR << "Trying to set JoystickButton trigger on keyboard+mouse control";
        return;
    }
    config.joystickButton = but;
    config.type           = Type::JoystickButton;
}

void Trigger::triggerOnJoystickAxis(sf::Joystick::Axis a, bool pos) {
    if (!joystickMode) {
        BL_LOG_ERROR << "Trying to set JoystickAxis trigger on keyboard+mouse control";
        return;
    }
    config.joystickAxis = a;
    config.type         = pos ? Type::JoystickPositive : Type::JoystickNegative;
}

void Trigger::triggerOnConfig(const Config& cfg) {
    switch (cfg.type) {
    case Type::Key:
    case Type::MouseButton:
    case Type::MouseWheelUp:
    case Type::MouseWheelDown:
        if (joystickMode) {
            BL_LOG_ERROR << "Trying to set non-joystick trigger on joystick control";
            return;
        }
        break;
    case Type::JoystickButton:
    case Type::JoystickPositive:
    case Type::JoystickNegative:
        if (!joystickMode) {
            BL_LOG_ERROR << "Trying to set joystick trigger on keyboard+mouse control";
            return;
        }
        break;
    default:
        BL_LOG_ERROR << "Trying to set invalid trigger type";
        return;
    }
    config = cfg;
}

bool Trigger::active() const { return nowActive; }

bool Trigger::process(const sf::Event& event) {
    bool result = false;
    event.visit(util::Visitor{
        [this, &result](const sf::Event::KeyPressed& keyEvent) {
            if (config.type == Type::Key && keyEvent.code == config.key) {
                result = makeOrtoggleActive();
            }
        },
        [this](const sf::Event::KeyReleased& keyEvent) {
            if (config.type == Type::Key && keyEvent.code == config.key) { makeInactive(); }
        },
        [this, &result](const sf::Event::MouseButtonPressed& mbEvent) {
            if (config.type == Type::MouseButton && mbEvent.button == config.mouseButton) {
                result = makeOrtoggleActive();
            }
        },
        [this](const sf::Event::MouseButtonReleased& mbEvent) {
            if (config.type == Type::MouseButton && mbEvent.button == config.mouseButton) {
                makeInactive();
            }
        },
        [this, &result](const sf::Event::MouseWheelScrolled& mwEvent) {
            if (mwEvent.wheel == config.mouseWheel) {
                if (config.type == Type::MouseWheelUp && mwEvent.delta > 0.f) {
                    if (config.toggle) {
                        nowActive = !nowActive;
                        result    = nowActive;
                    }
                    else { result = true; }
                }
                else if (config.type == Type::MouseWheelDown && mwEvent.delta < 0.f) {
                    if (config.toggle) {
                        nowActive = !nowActive;
                        result    = nowActive;
                    }
                    else { result = true; }
                }
            }
        },
        [this, &result](const sf::Event::JoystickButtonPressed& jbEvent) {
            if (config.type == Type::JoystickButton && jbEvent.button == config.joystickButton) {
                result = makeOrtoggleActive();
            }
        },
        [this](const sf::Event::JoystickButtonReleased& jbEvent) {
            if (config.type == Type::JoystickButton && jbEvent.button == config.joystickButton) {
                makeInactive();
            }
        },
        [this, &result](const sf::Event::JoystickMoved& jmEvent) {
            if (jmEvent.axis == config.joystickAxis) {
                if (config.type == Type::JoystickPositive && jmEvent.position >= StickThreshold) {
                    result = makeOrtoggleActive();
                }
                else if (config.type == Type::JoystickNegative &&
                         jmEvent.position <= -StickThreshold) {
                    result = makeOrtoggleActive();
                }
                else { makeInactive(); }
            }
        },
        [](const auto&) {
            // Handle any other event types
        }});
    return result;
}

bool Trigger::configureFromEvent(const sf::Event& event) {
    bool result = false;
    event.visit(util::Visitor{[this, &result](const sf::Event::KeyPressed& keyEvent) {
                                  if (!joystickMode) {
                                      config.type = Type::Key;
                                      config.key  = keyEvent.code;
                                      result      = true;
                                  }
                              },
                              [this, &result](const sf::Event::MouseButtonPressed& mbEvent) {
                                  if (!joystickMode) {
                                      config.type        = Type::MouseButton;
                                      config.mouseButton = mbEvent.button;
                                      result             = true;
                                  }
                              },
                              [this, &result](const sf::Event::MouseWheelScrolled& mwEvent) {
                                  if (!joystickMode) {
                                      config.type       = mwEvent.delta > 0.f ? Type::MouseWheelUp :
                                                                                Type::MouseWheelDown;
                                      config.mouseWheel = mwEvent.wheel;
                                      result            = true;
                                  }
                              },
                              [this, &result](const sf::Event::JoystickButtonPressed& jbEvent) {
                                  if (joystickMode) {
                                      config.type           = Type::JoystickButton;
                                      config.joystickButton = jbEvent.button;
                                      result                = true;
                                  }
                              },
                              [this, &result](const sf::Event::JoystickMoved& jmEvent) {
                                  if (joystickMode) {
                                      if (jmEvent.position >= StickThreshold) {
                                          config.type         = Type::JoystickPositive;
                                          config.joystickAxis = jmEvent.axis;
                                          result              = true;
                                      }
                                      else if (jmEvent.position <= -StickThreshold) {
                                          config.type         = Type::JoystickNegative;
                                          config.joystickAxis = jmEvent.axis;
                                          result              = true;
                                      }
                                  }
                              },
                              [](const auto&) {
                                  // Handle any other event types
                              }});
    return result;
}

void Trigger::saveToConfig(const std::string& prefix) const {
    if (config.type == Type::Invalid) {
        BL_LOG_WARN << "Refusing to save unset control: " << prefix;
        return;
    }
    bl::engine::Configuration::set<bool>(toggleKey(prefix), config.toggle);
    bl::engine::Configuration::set<std::string>(inputKey(prefix), toString());
}

void Trigger::loadFromConfig(const std::string& prefix) {
    using T = Encoder::ControlInfo::Type;

    const std::string str =
        bl::engine::Configuration::getOrDefault<std::string>(inputKey(prefix), "");
    if (str.empty()) return; // don't clobber default state if no config
    const Encoder::ControlInfo ctrl = Encoder::fromString(str);
    config.toggle = bl::engine::Configuration::getOrDefault<bool>(toggleKey(prefix));

    switch (ctrl.type) {
    case T::Key:
        config.type = Type::Key;
        config.key  = ctrl.key;
        break;

    case T::MouseButton:
        config.type        = Type::MouseButton;
        config.mouseButton = ctrl.mouseButton;
        break;

    case T::MouseWheelUp:
        config.type       = Type::MouseWheelUp;
        config.mouseWheel = ctrl.mouseWheel;
        break;

    case T::MouseWheelDown:
        config.type       = Type::MouseWheelDown;
        config.mouseWheel = ctrl.mouseWheel;
        break;

    case T::JoystickButton:
        config.type           = Type::JoystickButton;
        config.joystickButton = ctrl.joystickButton;
        break;

    case T::JoystickAxisPositive:
        config.type         = Type::JoystickPositive;
        config.joystickAxis = ctrl.joystickAxis;
        break;

    case T::JoystickAxisNegative:
        config.type         = Type::JoystickNegative;
        config.joystickAxis = ctrl.joystickAxis;
        break;

    default:
        config.type = Type::Invalid;
        break;
    }
}

std::string Trigger::toString() const {
    Encoder::ControlInfo ctrl;
    switch (config.type) {
    case Type::Key:
        ctrl = Encoder::ControlInfo(config.key);
        break;
    case Type::MouseButton:
        ctrl = Encoder::ControlInfo(config.mouseButton);
        break;
    case Type::MouseWheelUp:
        ctrl = Encoder::ControlInfo(config.mouseWheel, true);
        break;
    case Type::MouseWheelDown:
        ctrl = Encoder::ControlInfo(config.mouseWheel, false);
        break;
    case Type::JoystickButton:
        ctrl = Encoder::ControlInfo(config.joystickButton);
        break;
    case Type::JoystickPositive:
        ctrl = Encoder::ControlInfo(config.joystickAxis, true);
        break;
    case Type::JoystickNegative:
        ctrl = Encoder::ControlInfo(config.joystickAxis, false);
        break;
    case Type::Invalid:
    default:
        return "UNSET";
        break;
    }

    return Encoder::toString(ctrl);
}

Trigger::Type Trigger::triggerType() const { return config.type; }

bool Trigger::isToggle() const { return config.toggle; }

void Trigger::setIsToggle(bool t) { config.toggle = t; }

sf::Keyboard::Key Trigger::keyTrigger() const { return config.key; }

sf::Mouse::Button Trigger::mouseButtonTrigger() const { return config.mouseButton; }

sf::Mouse::Wheel Trigger::mouseWheelTrigger() const { return config.mouseWheel; }

unsigned int Trigger::joystickButtonTrigger() const { return config.joystickButton; }

sf::Joystick::Axis Trigger::joystickAxisTrigger() const { return config.joystickAxis; }

bool Trigger::makeOrtoggleActive() {
    const bool wasActive = nowActive;
    if (config.toggle) { nowActive = !nowActive; }
    else { nowActive = true; }
    return nowActive && !wasActive;
}

void Trigger::makeInactive() {
    if (!config.toggle) { nowActive = false; }
}

} // namespace input
} // namespace bl

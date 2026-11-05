#include <BLIB/Input/Control.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace input
{
Control::Control(Type t, bool jm)
: isJoystickControl(jm)
, type(t) {
    switch (type) {
    case Type::Movement:
        new (&movementControl) MovementControl(isJoystickControl);
        break;
    case Type::Directional:
        new (&directionalControl) DirectionalControl(isJoystickControl);
        break;
    case Type::SingleTrigger:
    default:
        new (&triggerControl) Trigger(isJoystickControl);
        break;
    }
}

void Control::configure(Type t, const Config& config) {
    type = t;
    switch (type) {
    case Type::Movement:
        new (&movementControl) MovementControl(isJoystickControl);
        movementControl.configure(config.movement);
        break;
    case Type::Directional:
        new (&directionalControl) DirectionalControl(isJoystickControl);
        directionalControl.config = config.directional;
        break;
    case Type::SingleTrigger:
        new (&triggerControl) Trigger(isJoystickControl);
        triggerControl.triggerOnConfig(config.trigger);
        break;
    default:
        BL_LOG_ERROR << "Invalid control type when configuring";
        break;
    }
}

void Control::saveToConfig(const std::string& prefix) const {
    switch (type) {
    case Type::Movement:
        movementControl.saveToConfig(prefix);
        break;
    case Type::Directional:
        directionalControl.saveToConfig(prefix);
        break;
    case Type::SingleTrigger:
        triggerControl.saveToConfig(prefix);
        break;
    }
}

void Control::loadFromConfig(const std::string& prefix) {
    switch (type) {
    case Type::Movement:
        movementControl.loadFromConfig(prefix);
        break;
    case Type::Directional:
        directionalControl.loadFromConfig(prefix);
        break;
    case Type::SingleTrigger:
        triggerControl.loadFromConfig(prefix);
        break;
    }
}

std::string Control::toString() const {
    switch (type) {
    case Type::Movement:
        return movementControl.toString();
        break;
    case Type::Directional:
        return directionalControl.toString();
        break;
    case Type::SingleTrigger:
        return triggerControl.toString();
        break;
    }
    return "INVALID";
}

} // namespace input
} // namespace bl

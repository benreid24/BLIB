#include <BLIB/Input/Control.hpp>

namespace bl
{
namespace input
{
Control::Control(Type t, bool jm)
: type(t) {
    switch (type) {
    case Type::Movement:
        new (&movementControl) MovementControl(jm);
        break;
    case Type::Directional:
        new (&directionalControl) DirectionalControl(jm);
        break;
    case Type::SingleTrigger:
    default:
        new (&triggerControl) Trigger(jm);
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

} // namespace input
} // namespace bl

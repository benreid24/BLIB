#include <BLIB/Input/Actor.hpp>

#include <BLIB/Logging.hpp>
#include <Input/Consts.hpp>

namespace bl
{
namespace input
{
namespace
{
std::string kbmPrefix(const std::string& prefix, unsigned int i) {
    return prefix + ".control-" + std::to_string(i) + ".kbm";
}

std::string joystickPrefix(const std::string& prefix, unsigned int i) {
    return prefix + ".control-" + std::to_string(i) + ".joystick";
}
} // namespace

Actor::Actor(InputSystem& owner, const std::vector<Control::Type>& schema)
: owner(owner)
, joystick(sf::Joystick::Count)
, activeControls(&kbmControls) {
    resetControls(schema);
    listeners.reserve(8);
}

void Actor::resetControls(const std::vector<Control::Type>& schema) {
    kbmControls.reserve(schema.size());
    joystickControls.reserve(schema.size());
    for (const Control::Type ctype : schema) {
        kbmControls.emplace_back(ctype, false);
        joystickControls.emplace_back(ctype, true);
    }
}

Actor::~Actor() {
    for (Listener* listener : listeners) { listener->owner = nullptr; }
}

void Actor::assignJoystick(unsigned int j) { joystick = j; }

Trigger& Actor::getKBMTriggerControl(unsigned int c) { return kbmControls[c].triggerControl; }

const Trigger& Actor::getKBMTriggerControl(unsigned int c) const {
    return kbmControls[c].triggerControl;
}

Trigger& Actor::getKBMMovementUpControl(unsigned int c) {
    return kbmControls[c].movementControl.upControl();
}

Trigger& Actor::getKBMMovementRightControl(unsigned int c) {
    return kbmControls[c].movementControl.rightControl();
}

Trigger& Actor::getKBMMovementDownControl(unsigned int c) {
    return kbmControls[c].movementControl.downControl();
}

Trigger& Actor::getKBMMovementLeftControl(unsigned int c) {
    return kbmControls[c].movementControl.leftControl();
}

const Trigger& Actor::getKBMMovementUpControl(unsigned int c) const {
    return kbmControls[c].movementControl.upControl();
}

const Trigger& Actor::getKBMMovementRightControl(unsigned int c) const {
    return kbmControls[c].movementControl.rightControl();
}

const Trigger& Actor::getKBMMovementDownControl(unsigned int c) const {
    return kbmControls[c].movementControl.downControl();
}

const Trigger& Actor::getKBMMovementLeftControl(unsigned int c) const {
    return kbmControls[c].movementControl.leftControl();
}

Trigger& Actor::getJoystickTriggerControl(unsigned int c) {
    return joystickControls[c].triggerControl;
}

const Trigger& Actor::getJoystickTriggerControl(unsigned int c) const {
    return joystickControls[c].triggerControl;
}

Joystick& Actor::getJoystickMovementControl(unsigned int c) {
    return joystickControls[c].movementControl.joystickControl();
}

const Joystick& Actor::getJoystickMovementControl(unsigned int c) const {
    return joystickControls[c].movementControl.joystickControl();
}

Joystick& Actor::getJoystickDirectionalControl(unsigned int c) {
    return joystickControls[c].directionalControl.joystick;
}

const Joystick& Actor::getJoystickDirectionalControl(unsigned int c) const {
    return joystickControls[c].directionalControl.joystick;
}

bool Actor::joystickMode() const { return activeControls == &joystickControls; }

bool Actor::controlActive(unsigned int c) const {
    if ((*activeControls)[c].type != Control::Type::SingleTrigger) {
        BL_LOG_ERROR << "Reading bad control value: " << c;
        return false;
    }
    return (*activeControls)[c].triggerControl.active();
}

sf::Vector2f Actor::readControl(unsigned int c) const {
    switch ((*activeControls)[c].type) {
    case Control::Type::Directional:
        return (*activeControls)[c].directionalControl.normalizedDirection;
    case Control::Type::Movement:
        return (*activeControls)[c].movementControl.readValue();
    default:
        BL_LOG_ERROR << "Reading bad control value: " << c;
        return {0.f, 0.f};
    }
}

void Actor::update() {
    for (unsigned int i = 0; i < (*activeControls).size(); ++i) {
        const Control& ctrl = (*activeControls)[i];

        switch (ctrl.type) {
        case Control::Type::SingleTrigger:
            if (ctrl.triggerControl.active()) {
                dispatch(i, DispatchType::TriggerActivated, false);
            }
            break;
        case Control::Type::Movement:
            if (ctrl.movementControl.type == MovementControl::Type::Triggers) {
                const auto& keys = ctrl.movementControl.keys;
                if (keys.up.active()) { dispatch(i, DispatchType::MovementUp, false); }
                if (keys.right.active()) { dispatch(i, DispatchType::MovementRight, false); }
                if (keys.down.active()) { dispatch(i, DispatchType::MovementDown, false); }
                if (keys.left.active()) { dispatch(i, DispatchType::MovementLeft, false); }
            }
            else {
                const sf::Vector2f& pos = ctrl.movementControl.joystick.cachedPosition;
                if (pos.x >= StickThreshold) { dispatch(i, DispatchType::MovementRight, false); }
                if (pos.x <= -StickThreshold) { dispatch(i, DispatchType::MovementLeft, false); }
                if (pos.y >= StickThreshold) { dispatch(i, DispatchType::MovementUp, false); }
                if (pos.y <= -StickThreshold) { dispatch(i, DispatchType::MovementDown, false); }
            }
            break;
        default:
            break;
        }
    }
}

void Actor::addListener(Listener& l) {
    if (l.owner != this) {
        if (l.owner != nullptr) { l.owner->removeListener(l); }
        listeners.emplace_back(&l);
        l.owner = this;
    }
}

void Actor::removeListener(Listener& l) {
    for (auto it = listeners.rbegin(); it != listeners.rend(); ++it) {
        if (*it == &l) {
            listeners.erase(--(it.base()));
            break;
        }
    }
    l.owner = nullptr;
}

void Actor::replaceListener(Listener& old, Listener& l) {
    for (auto it = listeners.rbegin(); it != listeners.rend(); ++it) {
        if (*it == &old) {
            *it = &l;
            break;
        }
    }
    l.owner   = this;
    old.owner = nullptr;
}

void Actor::dispatch(unsigned int ctrl, DispatchType dt, bool onEvent) {
    for (auto it = listeners.rbegin(); it != listeners.rend(); ++it) {
        if ((*it)->observe(*this, ctrl, dt, onEvent)) break;
    }
}

void Actor::process(const sf::Event& event) {
    // determine if keyboard or controller is being used
    switch (event.type) {
    case sf::Event::JoystickButtonPressed:
    case sf::Event::JoystickButtonReleased:
        if (joystick < sf::Joystick::Count && event.joystickButton.joystickId != joystick) return;
        activeControls = &joystickControls;
        break;
    case sf::Event::JoystickMoved:
        if (joystick < sf::Joystick::Count && event.joystickMove.joystickId != joystick) return;
        activeControls = &joystickControls;
        break;
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
    case sf::Event::MouseButtonPressed:
    case sf::Event::MouseButtonReleased:
    case sf::Event::MouseWheelScrolled:
    case sf::Event::MouseMoved:
        activeControls = &kbmControls;
        break;
    default:
        return; // can exit early, event does not trigger any controls
    }

    // process control
    for (unsigned int i = 0; i < (*activeControls).size(); ++i) {
        Control& ctrl = (*activeControls)[i];

        switch (ctrl.type) {
        case Control::Type::SingleTrigger:
            if (ctrl.triggerControl.process(event)) {
                dispatch(i, DispatchType::TriggerActivated, true);
            }
            break;
        case Control::Type::Movement: {
            const DispatchType dt = ctrl.movementControl.process(event);
            if (dt != DispatchType::_INVALID) { dispatch(i, dt, true); }
        } break;
        case Control::Type::Directional:
            if (ctrl.directionalControl.process(owner, event)) {
                dispatch(i, DispatchType::DirectionChanged, true);
            }
            break;
        default:
            break;
        }
    }
}

void Actor::saveToConfig(const std::string& prefix) const {
    for (unsigned int i = 0; i < kbmControls.size(); ++i) {
        kbmControls[i].saveToConfig(kbmPrefix(prefix, i));
    }
    for (unsigned int i = 0; i < joystickControls.size(); ++i) {
        joystickControls[i].saveToConfig(joystickPrefix(prefix, i));
    }
}

void Actor::loadFromConfig(const std::string& prefix) {
    for (unsigned int i = 0; i < kbmControls.size(); ++i) {
        kbmControls[i].loadFromConfig(kbmPrefix(prefix, i));
    }
    for (unsigned int i = 0; i < joystickControls.size(); ++i) {
        joystickControls[i].loadFromConfig(joystickPrefix(prefix, i));
    }
}

} // namespace input
} // namespace bl

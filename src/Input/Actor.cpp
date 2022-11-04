#include <BLIB/Input/Actor.hpp>

#include <BLIB/Logging.hpp>
#include <Input/Consts.hpp>

namespace bl
{
namespace input
{
Actor::Actor(InputSystem& owner, const std::vector<Control::Type>& schema)
: owner(owner)
, joystick(sf::Joystick::Count) {
    controls.reserve(schema.size());
    for (const Control::Type ctype : schema) { controls.emplace_back(ctype); }
    listeners.reserve(8);
}

Actor::~Actor() {
    for (Listener* listener : listeners) { listener->owner = nullptr; }
}

void Actor::assignJoystick(unsigned int j) { joystick = j; }

Trigger& Actor::getTriggerControl(unsigned int c) { return controls[c].triggerControl; }

const Trigger& Actor::getTriggerControl(unsigned int c) const { return controls[c].triggerControl; }

MovementControl& Actor::getMovementControl(unsigned int c) { return controls[c].movementControl; }

const MovementControl& Actor::getMovementControl(unsigned int c) const {
    return controls[c].movementControl;
}

DirectionalControl& Actor::getDirectionalControl(unsigned int c) {
    return controls[c].directionalControl;
}

const DirectionalControl& Actor::getDirectionalControl(unsigned int c) const {
    return controls[c].directionalControl;
}

bool Actor::controlActive(unsigned int c) const {
    if (controls[c].type != Control::Type::SingleTrigger) {
        BL_LOG_ERROR << "Reading bad control value: " << c;
        return false;
    }
    return controls[c].triggerControl.active();
}

sf::Vector2f Actor::readControl(unsigned int c) const {
    switch (controls[c].type) {
    case Control::Type::Directional:
        return controls[c].directionalControl.normalizedDirection;
    case Control::Type::Movement:
        return controls[c].movementControl.readValue();
    default:
        BL_LOG_ERROR << "Reading bad control value: " << c;
        return {0.f, 0.f};
    }
}

void Actor::update() {
    for (unsigned int i = 0; i < controls.size(); ++i) {
        const Control& ctrl = controls[i];

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
                if (pos.x <= StickThreshold) { dispatch(i, DispatchType::MovementLeft, false); }
                if (pos.y >= StickThreshold) { dispatch(i, DispatchType::MovementUp, false); }
                if (pos.y <= StickThreshold) { dispatch(i, DispatchType::MovementDown, false); }
            }
            break;
        default:
            break;
        }
    }
}

void Actor::addListener(Listener& l) {
    listeners.emplace_back(&l);
    l.owner = this;
}

void Actor::removeListener(Listener& l) {
    for (int i = listeners.size(); i >= 0; --i) {
        if (listeners[i] == &l) {
            listeners.erase(listeners.begin() + i);
            break;
        }
    }
    l.owner = nullptr;
}

void Actor::replaceListener(Listener& old, Listener& l) {
    for (int i = listeners.size(); i >= 0; --i) {
        if (listeners[i] == &old) {
            listeners[i] = &l;
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
    // filter joystick events if listening to specific joystick
    if (joystick < sf::Joystick::Count) {
        switch (event.type) {
        case sf::Event::JoystickButtonPressed:
        case sf::Event::JoystickButtonReleased:
            if (event.joystickButton.joystickId != joystick) return;
            break;
        case sf::Event::JoystickMoved:
            if (event.joystickMove.joystickId != joystick) return;
            break;
        default:
            break;
        }
    }

    for (unsigned int i = 0; i < controls.size(); ++i) {
        Control& ctrl = controls[i];

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

} // namespace input
} // namespace bl

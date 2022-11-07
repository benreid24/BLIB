#include <BLIB/Input/Configurator.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace input
{
namespace
{
constexpr float AxisPosThreshold = 90.f;
constexpr float AxisNegThreshold = 90.f;
} // namespace

Configurator::Configurator(event::Dispatcher& b)
: state(Finished)
, bus(b)
, trigger(nullptr)
, axis(sf::Joystick::Axis::X) {}

void Configurator::start(Trigger& t) {
    if (!finished()) {
        BL_LOG_ERROR << "Cannot start configurator, already running";
        return;
    }
    state   = WaitingTrigger;
    trigger = &t;
    bus.subscribe(this);
}

void Configurator::start(Joystick& js) {
    if (!finished()) {
        BL_LOG_ERROR << "Cannot start configurator, already running";
        return;
    }
    state    = WaitingHorAxis;
    joystick = &js;
    jsState  = JoystickState::WaitingPositive;
    bus.subscribe(this);
}

bool Configurator::finished() const { return state == Finished; }

Configurator::State Configurator::getState() const { return state; }

void Configurator::observe(const sf::Event& event) {
    switch (state) {
    case WaitingTrigger:
        if (trigger->configureFromEvent(event)) {
            bus.unsubscribe(this, true);
            state = Finished;
        }
        break;

    case WaitingHorAxis:
    case WaitingVertAxis:
        switch (jsState) {
        case JoystickState::WaitingPositive:
            if (event.type == sf::Event::JoystickMoved) {
                if (event.joystickMove.position >= AxisPosThreshold) {
                    axis    = event.joystickMove.axis;
                    jsState = JoystickState::WaitingNegative;
                }
            }
            break;
        case JoystickState::WaitingNegative:
            if (event.type == sf::Event::JoystickMoved) {
                if (event.joystickMove.position <= AxisNegThreshold) {
                    if (axis == event.joystickMove.axis) {
                        if (state == WaitingHorAxis) {
                            joystick->horizontalAxis = axis;
                            state                    = WaitingVertAxis;
                            jsState                  = JoystickState::WaitingPositive;
                        }
                        else {
                            joystick->verticalAxis = axis;
                            bus.unsubscribe(this, true);
                            state = Finished;
                        }
                    }
                    else {
                        jsState = JoystickState::WaitingPositive;
                    }
                }
            }
            break;
        }
        break;

    default:
        BL_LOG_ERROR << "Received event while not actively configuring control";
        break;
    }
}

} // namespace input
} // namespace bl

#include <BLIB/Input/Configurator.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/Visitor.hpp>

namespace bl
{
namespace input
{
namespace
{
constexpr float AxisPosThreshold = 90.f;
constexpr float AxisNegThreshold = 90.f;
} // namespace

Configurator::Configurator()
: state(Finished)
, trigger(nullptr)
, axis(sf::Joystick::Axis::X) {}

void Configurator::start(engine::Engine& engine, Trigger& t) {
    if (!finished()) {
        BL_LOG_ERROR << "Cannot start configurator, already running";
        return;
    }
    state   = WaitingTrigger;
    trigger = &t;
    subscribeDeferred(engine.getSignalChannel());
}

void Configurator::start(engine::Engine& engine, Joystick& js) {
    if (!finished()) {
        BL_LOG_ERROR << "Cannot start configurator, already running";
        return;
    }
    state    = WaitingHorAxis;
    joystick = &js;
    jsState  = JoystickState::WaitingPositive;
    subscribeDeferred(engine.getSignalChannel());
}

bool Configurator::finished() const { return state == Finished; }

Configurator::State Configurator::getState() const { return state; }

void Configurator::process(const sf::Event& event) {
    if (state == WaitingTrigger) {
        if (trigger->configureFromEvent(event)) {
            unsubscribeDeferred();
            state = Finished;
        }
        return;
    }

    if (state == WaitingHorAxis || state == WaitingVertAxis) {
        event.visit(util::Visitor{
            [this](const sf::Event::JoystickMoved& jmEvent) {
                if (jsState == JoystickState::WaitingPositive) {
                    if (jmEvent.position >= AxisPosThreshold) {
                        axis    = jmEvent.axis;
                        jsState = JoystickState::WaitingNegative;
                    }
                }
                else if (jsState == JoystickState::WaitingNegative) {
                    if (jmEvent.position <= AxisNegThreshold) {
                        if (axis == jmEvent.axis) {
                            if (state == WaitingHorAxis) {
                                joystick->horizontalAxis = axis;
                                state                    = WaitingVertAxis;
                                jsState                  = JoystickState::WaitingPositive;
                            }
                            else {
                                joystick->verticalAxis = axis;
                                unsubscribeDeferred();
                                state = Finished;
                            }
                        }
                        else { jsState = JoystickState::WaitingPositive; }
                    }
                }
            },
            [](const auto&) {
                // Handle any other event types
            }});
        return;
    }

    BL_LOG_ERROR << "Received event while not actively configuring control";
}

} // namespace input
} // namespace bl

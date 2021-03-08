#include <BLIB/Engine/Event.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
Event::Event(const StartupEvent& event)
: type(Type::Startup)
, startup(event) {}

Event::Event(const ShutdownEvent& event)
: type(Type::Shutdown)
, shutdown(event) {}

Event::Event(const StateChangeEvent& event)
: type(Type::StateChange)
, stateChange(event) {}

Event::Event(const PausedEvent& event)
: type(Type::EnginePaused)
, paused(event) {}

Event::Event(const ResumedEvent& event)
: type(Type::EngineResumed)
, resumed(event) {}

Event::~Event() {
    switch (type) {
    case Type::Startup:
        startup.~StartupEvent();
        break;
    case Type::Shutdown:
        shutdown.~ShutdownEvent();
        break;
    case Type::StateChange:
        stateChange.~StateChangeEvent();
        break;
    case Type::EnginePaused:
        paused.~PausedEvent();
        break;
    case Type::EngineResumed:
        resumed.~ResumedEvent();
        break;
    default:
        BL_LOG_WARN << "Invalid engine event type destructed: " << type;
        break;
    }
}

} // namespace engine
} // namespace bl

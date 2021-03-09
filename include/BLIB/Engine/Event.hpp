#ifndef BLIB_ENGINE_EVENT_HPP
#define BLIB_ENGINE_EVENT_HPP

#include <BLIB/Engine/State.hpp>

namespace bl
{
namespace engine
{
class Engine;

/**
 * @brief Basic event class for engine related events
 *
 * @ingroup Engine
 *
 */
struct Event {
public:
    /**
     * @brief A struct wrapper around the EventType enum for scope encapsulation
     *
     */
    struct Type {
        enum EventType {
            /// The engine has started up and the main loop is about to execute
            Startup,

            /// The engine is exiting and the main loop has terminated
            Shutdown,

            /// A new engine state was activated
            StateChange,

            /// The engine is pausing to await window focus
            EnginePaused,

            /// The engine has regained focus and will resume
            EngineResumed
        };
    };

    /// Stores the type of event this is
    const Type::EventType type;

    /// Stores data related to Startup event type
    struct StartupEvent {
        State::Ptr initialState;
    };

    /// Stores data related to Shutdown event type
    struct ShutdownEvent {
        /// What triggered the engine to shutdown
        enum ShutdownType {
            /// The terminate engine flag was set
            Terminated,

            /// The final engine state was popped off
            FinalStatePopped,

            /// The main window was closed
            WindowClosed
        } cause;
    };

    /// Stores data related to StateChange event type
    struct StateChangeEvent {
        /// The new state of the engine
        State::Ptr newState;

        /// The previous state of the engine. May be null
        State::Ptr prevState;
    };

    /// Empty struct to indicate the engine has paused
    struct PausedEvent {};

    /// Empty struct to indicate the engine has resumed
    struct ResumedEvent {};

    union {
        /// Contains startup information
        StartupEvent startup;

        /// Contains shutdown information
        ShutdownEvent shutdown;

        /// Contains state change information
        StateChangeEvent stateChange;

        /// Contains engine pause information
        PausedEvent paused;

        /// Contains engine resume information
        ResumedEvent resumed;
    };

    /// Copies an Event
    Event(const Event& copy);

    /// Cleans up
    ~Event();

private:
    Event(const StartupEvent& event);
    Event(const ShutdownEvent& event);
    Event(const StateChangeEvent& event);
    Event(const PausedEvent& event);
    Event(const ResumedEvent& event);

    friend class Engine;
};

} // namespace engine
} // namespace bl

#endif

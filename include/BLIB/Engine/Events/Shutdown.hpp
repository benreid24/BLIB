#ifndef BLIB_ENGINE_EVENTS_SHUTDOWN_HPP
#define BLIB_ENGINE_EVENTS_SHUTDOWN_HPP

namespace bl
{
namespace engine
{
namespace event
{
/**
 * @brief Fired when the Engine shuts down
 *
 * @ingroup EngineEvents
 *
 */
struct Shutdown {
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

} // namespace event
} // namespace engine
} // namespace bl

#endif

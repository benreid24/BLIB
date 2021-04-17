#ifndef BLIB_ENGINE_EVENTS_STARTUP_HPP
#define BLIB_ENGINE_EVENTS_STARTUP_HPP

#include <BLIB/Engine/State.hpp>

namespace bl
{
namespace engine
{
/// Collection of Engine events
namespace event
{
/**
 * @brief Fired when the engine is first started
 *
 * @ingroup EngineEvents
 */
struct Startup {
    /// The State the engine has started in
    State::Ptr initialState;
};

} // namespace event
} // namespace engine
} // namespace bl

#endif

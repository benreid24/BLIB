#ifndef BLIB_ENGINE_EVENTS_STATECHANGED_HPP
#define BLIB_ENGINE_EVENTS_STATECHANGED_HPP

namespace bl
{
namespace engine
{
namespace event
{
/**
 * @brief Fired when the engine switches states
 *
 * @ingroup EngineEvents
 *
 */
struct StateChange {
    /// The new state of the engine
    State::Ptr newState;

    /// The previous state of the engine. May be null
    State::Ptr prevState;
};

} // namespace event
} // namespace engine
} // namespace bl

#endif

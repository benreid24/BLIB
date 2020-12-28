#ifndef BLIB_ENGINE_ENGINEEVENT_HPP
#define BLIB_ENGINE_ENGINEEVENT_HPP

#include <BLIB/Util/EventDispatcher.hpp>

namespace bl
{
/**
 * @brief Basic event class for engine related events
 *
 * @ingroup Engine
 *
 */
struct EngineEvent {
public:
    // TODO
};

using EngineEventDispatcher = EventDispatcher<EngineEvent>;
using EngineEventListener   = EventListener<EngineEvent>;

} // namespace bl

#endif

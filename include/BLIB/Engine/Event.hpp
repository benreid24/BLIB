#ifndef BLIB_ENGINE_EVENT_HPP
#define BLIB_ENGINE_EVENT_HPP

#include <BLIB/Util/EventDispatcher.hpp>

namespace bl
{
namespace engine
{
/**
 * @brief Basic event class for engine related events
 *
 * @ingroup Engine
 *
 */
struct Event {
public:
    // TODO
};

using EventDispatcher = bl::EventDispatcher<Event>;
using EventListener   = bl::EventListener<Event>;

} // namespace engine
} // namespace bl

#endif

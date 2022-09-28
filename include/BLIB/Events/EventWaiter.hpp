#ifndef BLIB_EVENTS_EVENTWAITER_HPP
#define BLIB_EVENTS_EVENTWAITER_HPP

#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Events/Listener.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/Waiter.hpp>
#include <optional>

namespace bl
{
namespace event
{
/**
 * @brief Utility class to block until a specific event is fired on the bus
 *
 * @tparam T The type of event to block for
 * @ingroup Events
 */
template<typename T>
class EventWaiter
: public Listener<T>
, private util::NonCopyable {
public:
    /**
     * @brief Construct a new Event Waiter object
     *
     */
    EventWaiter() = default;

    /**
     * @brief Block until the correct type of event is fired. May be unblocked early on program
     *        termination if util::Waiter::unblockAll() is called
     *
     * @param bus The event bus to wait on
     * @return const std::optional<T>& The captured event if one was fired
     */
    const std::optional<T>& wait(Dispatcher& bus);

private:
    util::Waiter blocker;
    std::optional<T> event;

    virtual void observe(const T& event) override;
};

template<typename T>
const std::optional<T>& EventWaiter<T>::wait(Dispatcher& bus) {
    blocker.reset();
    bus.subscribe(this);
    blocker.wait();
    bus.unsubscribe(this);
    return event;
}

template<typename T>
void EventWaiter<T>::observe(const T& v) {
    event.emplace(v);
    blocker.unblock();
}

} // namespace event
} // namespace bl

#endif

#ifndef BLIB_EVENTS_DISPATCHER_HPP
#define BLIB_EVENTS_DISPATCHER_HPP

#include <BLIB/Events/Listener.hpp>
#include <BLIB/Events/SingleDispatcher.hpp>
#include <BLIB/Events/SubscribeHelpers.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/ReadWriteLock.hpp>

#include <mutex>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace bl
{
namespace event
{
namespace priv
{
struct ReserveOnStartupHelper;
}

/**
 * @brief Event dispatcher capable of handling events of many different types. The dispatcher is
 *        threadsafe. If events are dispatched by multiple threads care should be taken to keep
 *        the listeners threadsafe as well. Care must also be taken to not subscribe the same
 *        listener more than once or it will get notified more than once per event
 * @see Listener
 *
 * @ingroup Events
 *
 */
class Dispatcher : private util::NonCopyable {
public:
    /**
     * @brief Subscribe the given listener to the event stream for the event types given
     *
     * @tparam TEvents The types of events to subscribe to
     * @param listener The listener to receive events as they are dispatched
     * @param defer True to defer adding until syncListeners() is called. May miss events but can be
     *              called from within event listener observe() methods
     */
    template<typename... TEvents>
    static void subscribe(Listener<TEvents...>* listener, bool defer = false);

    /**
     * @brief Removes the given listener and prevents it from receiving any more events
     *
     * @tparam TEvents The types of events the listener is subscribed to
     * @param listener The listener to remove
     * @param defer True to defer adding until syncListeners() is called. May receive extra events
     *              but can be called from within event listener observe() methods
     */
    template<typename... TEvents>
    static void unsubscribe(Listener<TEvents...>* listener, bool defer = false);

    /**
     * @brief Dispatches the given event to each listener that is subscribed to that type of event
     *
     * @tparam T The type of event to dispatch
     * @param event The event to dispatch
     */
    template<typename T>
    static void dispatch(const T& event);

    /**
     * @brief Synchronizes the listener buses. Called once per update cycle by the engine
     *
     */
    static void syncListeners();

    /**
     * @brief Removes all listeners from the subscription queues. Meant mostly for unit test cleanup
     *        but may have other applications
     *
     */
    static void clearAllListeners();

private:
    static std::mutex dispatcherLock;
    static std::vector<priv::SingleDispatcherBase*> dispatchers;

    friend struct priv::ReserveOnStartupHelper;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TEvents>
void Dispatcher::subscribe(Listener<TEvents...>* listener, bool defer) {
    priv::SubscriberHelper<TEvents...> subscriber(dispatchers, dispatcherLock, listener, defer);
    listener->subscribed = true;
}

template<typename T>
void Dispatcher::dispatch(const T& event) {
    priv::SingleDispatcher<T>::get(dispatchers, dispatcherLock).dispatch(event);
}

template<typename... TEvents>
void Dispatcher::unsubscribe(Listener<TEvents...>* listener, bool defer) {
    priv::UnSubscriberHelper<TEvents...> subscriber(dispatchers, dispatcherLock, listener, defer);
    listener->subscribed = false;
}

} // namespace event
} // namespace bl

#include <BLIB/Events/ListenerImpl.hpp>

#endif

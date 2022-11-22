#ifndef BLIB_EVENTS_LISTENERGUARD_HPP
#define BLIB_EVENTS_LISTENERGUARD_HPP

#include <BLIB/Events/Dispatcher.hpp>

namespace bl
{
namespace event
{
/**
 * @brief Version of scope guard but for classes. Classes that subscribe to event buses through a
 *        member ListenerGuard will automatically unsubscribe when they are destructed
 *
 * @tparam TEvents The types of events to subscribe to
 * @ingroup Events
 */
template<typename... TEvents>
class ListenerGuard {
public:
    /**
     * @brief Creates an empty guard
     *
     * @param owner The owning class of this guard
     *
     */
    ListenerGuard(Listener<TEvents...>* owner);

    /**
     * @brief Unsubscribes if subscribed
     *
     */
    ~ListenerGuard();

    /**
     * @brief Subscribes the owner to the given bus
     *
     * @param bus Event bus to subscribe to
     * @param defer True to defer adding until the engine syncs listeners, false to immediately add
     */
    void subscribe(bool defer = false);

    /**
     * @brief Unsubscribes the owner from the given event bus
     *
     */
    void unsubscribe();

private:
    Listener<TEvents...>* const listener;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TEvents>
ListenerGuard<TEvents...>::ListenerGuard(Listener<TEvents...>* owner)
: listener(owner) {}

template<typename... TEvents>
ListenerGuard<TEvents...>::~ListenerGuard() {
    unsubscribe();
}

template<typename... TEvents>
void ListenerGuard<TEvents...>::subscribe(bool defer) {
    unsubscribe();
    Dispatcher::subscribe(listener, defer);
}

template<typename... TEvents>
void ListenerGuard<TEvents...>::unsubscribe() {
    Dispatcher::unsubscribe(listener);
}

} // namespace event
} // namespace bl

#endif

#ifndef BLIB_UTIL_EVENTDISPATCHERSCOPEGUARD_HPP
#define BLIB_UTIL_EVENTDISPATCHERSCOPEGUARD_HPP

#include <BLIB/Util/EventDispatcher.hpp>
#include <BLIB/Util/EventListener.hpp>

namespace bl
{
/**
 * @brief Scope guard for EventListener objects to subscribe to an EventDispatcher if they are
 *        limited by scope. All listeners subscribed through the guard are removed from the
 *        dispatcher when the guard goes out of scope
 *
 * @tparam T Type of event to handle
 * @ingroup Util
 */
template<typename T>
class EventDispatcherScopeGuard {
public:
    /**
     * @brief Create a new scope guard for the given dispatcher
     *
     */
    EventDispatcherScopeGuard(EventDispatcher<T>& dispatcher);

    /**
     * @brief Removes all added listeners from the dispatcher
     *
     */
    ~EventDispatcherScopeGuard();

    /**
     * @brief Subscribe the given scoped listener object
     *
     */
    void subscribe(EventListener<T>* listener);

    /**
     * @brief Remove the given listener
     *
     */
    void remove(EventListener<T>* listener);

private:
    EventDispatcher<T>& dispatcher;
    std::vector<EventListener<T>*> listeners;
};

typedef EventDispatcherScopeGuard<sf::Event> WindowEventDispatcherScopeGuard;

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
EventDispatcherScopeGuard<T>::EventDispatcherScopeGuard(EventDispatcher<T>& dispatcher)
: dispatcher(dispatcher) {}

template<typename T>
EventDispatcherScopeGuard<T>::~EventDispatcherScopeGuard() {
    for (unsigned int i = 0; i < listeners.size(); ++i) { dispatcher.remove(listeners[i]); }
}

template<typename T>
void EventDispatcherScopeGuard<T>::subscribe(EventListener<T>* listener) {
    listeners.push_back(listener);
    dispatcher.subscribe(listener);
}

template<typename T>
void EventDispatcherScopeGuard<T>::remove(EventListener<T>* listener) {
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
        listeners.erase(it);
        dispatcher.remove(listener);
    }
}

} // namespace bl

#endif

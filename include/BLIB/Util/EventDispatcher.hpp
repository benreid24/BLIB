#ifndef BLIB_UTIL_EVENTDISPATCHER_HPP
#define BLIB_UTIL_EVENTDISPATCHER_HPP

#include <BLIB/Util/EventListener.hpp>

namespace bl
{
/**
 * @brief Utility class to dispatch events to a set of listeners
 * @see EventListener
 *
 * @tparam T The type of event
 * @ingroup Util
 */
template<typename T>
class EventDispatcher {
public:
    /**
     * @brief Subscribe the given listener to receive events. Listener must not go out of scope
     *
     * @param listener The listener to start notifying
     */
    void subscribe(EventListener<T>* listener);

    /**
     * @brief Removes the given listener from the dispatcher queue. No effect if not present
     *
     * @param listener The listener to remove
     */
    void remove(EventListener<T>* listener);

    /**
     * @brief Dispatches the event to the queue of listeners
     *
     * @param event The event to dispatch
     */
    void dispatch(const T& event);

private:
    std::vector<EventListener<T>*> listeners;
};

/// Special instantiation of EventDispatcher for sf::Event window events
typedef EventDispatcher<sf::Event> WindowEventDispatcher;

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void EventDispatcher<T>::subscribe(EventListener<T>* listener) {
    listeners.push_back(listener);
}

template<typename T>
void EventDispatcher<T>::dispatch(const T& event) {
    for (EventListener<T>* listener : listeners) { listener->observe(event); }
}

template<typename T>
void EventDispatcher<T>::remove(EventListener<T>* listener) {
    for (unsigned int i = 0; i < listeners.size(); ++i) {
        if (listeners[i] == listener) {
            listeners.erase(listeners.begin() + i);
            --i;
        }
    }
}

} // namespace bl

#endif
#ifndef BLIB_EVENTS_SINGLEDISPATCHER_HPP
#define BLIB_EVENTS_SINGLEDISPATCHER_HPP

#include <BLIB/Events/SingleListener.hpp>
#include <mutex>
#include <shared_mutex>

namespace bl
{
namespace event
{
/**
 * @brief Utility class to dispatch events of a single type to a set of listeners
 * @see SingleListener
 *
 * @tparam T The type of event
 * @ingroup Events
 */
template<typename T>
class SingleDispatcher {
public:
    /**
     * @brief Subscribe the given listener to receive events. Listener must not go out of scope
     *
     * @param listener The listener to start notifying
     */
    void subscribe(SingleListener<T>* listener);

    /**
     * @brief Removes the given listener from the dispatcher queue. No effect if not present
     *
     * @param listener The listener to remove
     */
    void remove(SingleListener<T>* listener);

    /**
     * @brief Dispatches the event to the queue of listeners
     *
     * @param event The event to dispatch
     */
    void dispatch(const T& event);

private:
    std::shared_mutex mutex;
    std::vector<SingleListener<T>*> listeners;
};

/// Special instantiation of SingleDispatcher for sf::Event window events
typedef SingleDispatcher<sf::Event> WindowEventDispatcher;

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void SingleDispatcher<T>::subscribe(SingleListener<T>* listener) {
    std::unique_lock lock(mutex);
    listeners.push_back(listener);
}

template<typename T>
void SingleDispatcher<T>::dispatch(const T& event) {
    std::shared_lock lock(mutex);
    for (SingleListener<T>* listener : listeners) { listener->observe(event); }
}

template<typename T>
void SingleDispatcher<T>::remove(SingleListener<T>* listener) {
    std::unique_lock lock(mutex);
    for (unsigned int i = 0; i < listeners.size(); ++i) {
        if (listeners[i] == listener) {
            listeners.erase(listeners.begin() + i);
            --i;
        }
    }
}

} // namespace event
} // namespace bl

#endif

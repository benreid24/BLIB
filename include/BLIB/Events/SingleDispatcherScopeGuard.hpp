#ifndef BLIB_EVENTS_SINGLEDISPATCHERSCOPEGUARD_HPP
#define BLIB_EVENTS_SINGLEDISPATCHERSCOPEGUARD_HPP

#include <BLIB/Events/SingleDispatcher.hpp>
#include <BLIB/Events/SingleListener.hpp>

namespace bl
{
namespace event
{
/**
 * @brief Scope guard for SingleListener objects to subscribe to an SingleDispatcher if they are
 *        limited by scope. All listeners subscribed through the guard are removed from the
 *        dispatcher when the guard goes out of scope
 *
 * @tparam T Type of event to handle
 * @ingroup Events
 */
template<typename T>
class SingleDispatcherScopeGuard {
public:
    /**
     * @brief Create a new scope guard for the given dispatcher
     *
     */
    SingleDispatcherScopeGuard(SingleDispatcher<T>& dispatcher);

    /**
     * @brief Removes all added listeners from the dispatcher
     *
     */
    ~SingleDispatcherScopeGuard();

    /**
     * @brief Subscribe the given scoped listener object
     *
     */
    void subscribe(SingleListener<T>* listener);

    /**
     * @brief Remove the given listener
     *
     */
    void remove(SingleListener<T>* listener);

private:
    SingleDispatcher<T>& dispatcher;
    std::vector<SingleListener<T>*> listeners;
};

typedef SingleDispatcherScopeGuard<sf::Event> WindowEventDispatcherScopeGuard;

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
SingleDispatcherScopeGuard<T>::SingleDispatcherScopeGuard(SingleDispatcher<T>& dispatcher)
: dispatcher(dispatcher) {}

template<typename T>
SingleDispatcherScopeGuard<T>::~SingleDispatcherScopeGuard() {
    for (unsigned int i = 0; i < listeners.size(); ++i) { dispatcher.remove(listeners[i]); }
}

template<typename T>
void SingleDispatcherScopeGuard<T>::subscribe(SingleListener<T>* listener) {
    listeners.push_back(listener);
    dispatcher.subscribe(listener);
}

template<typename T>
void SingleDispatcherScopeGuard<T>::remove(SingleListener<T>* listener) {
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
        listeners.erase(it);
        dispatcher.remove(listener);
    }
}

} // namespace event
} // namespace bl

#endif

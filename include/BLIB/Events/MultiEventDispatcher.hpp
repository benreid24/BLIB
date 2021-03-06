#ifndef BLIB_EVENTS_MULTIEVENTDISPATCHER_HPP
#define BLIB_EVENTS_MULTIEVENTDISPATCHER_HPP

#include <BLIB/Events/MultiEventListener.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <shared_mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace bl
{
class MultiEventDispatcherScopeGuard;

/**
 * @brief Event dispatcher capable of handling events of many different types. The dispatcher is
 *        threadsafe. If events are dispatched by multiple threads care should be taken to keep
 *        the listeners threadsafe as well. Care must also be taken to not subscribe the same
 *        listener more than once or it will get notified more than once per event
 * @see MultiEventListener
 *
 * @ingroup Events
 *
 */
class MultiEventDispatcher : private NonCopyable {
public:
    /**
     * @brief Subscribe the given listener to the event stream for the event types given
     *
     * @tparam TEvents The types of events to subscribe to
     * @param listener The listener to receive events as they are dispatched
     */
    template<typename... TEvents>
    void subscribe(MultiEventListener<TEvents...>* listener);

    /**
     * @brief Removes the given listener and prevents it from receiving any more events
     *
     * @tparam TEvents The types of events the listener is subscribed to
     * @param listener The listener to remove
     */
    template<typename... TEvents>
    void unsubscribe(MultiEventListener<TEvents...>* listener);

    /**
     * @brief Dispatches the given event to each listener that is subscribed to that type of event
     *
     * @tparam T The type of event to dispatch
     * @param event The event to dispatch
     */
    template<typename T>
    void dispatch(const T& event) const;

private:
    mutable std::shared_mutex mutex;
    std::unordered_map<std::type_index, std::vector<void*>> listeners;

    void remove(const std::type_index& t, void* val);

    friend class MultiEventDispatcherScopeGuard;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TEvents>
void MultiEventDispatcher::subscribe(MultiEventListener<TEvents...>* listener) {
    std::type_index types[]  = {std::type_index(typeid(TEvents))...};
    void* const pointers[]   = {static_cast<MultiEventListenerBase<TEvents>*>(listener)...};
    constexpr std::size_t nt = sizeof...(TEvents);

    std::unique_lock lock(mutex);
    for (std::size_t i = 0; i < nt; ++i) {
        auto lit = listeners.find(types[i]);
        if (lit == listeners.end()) { lit = listeners.try_emplace(types[i]).first; }
        lit->second.emplace_back(pointers[i]);
    }
}

template<typename T>
void MultiEventDispatcher::dispatch(const T& event) const {
    const std::type_index type(typeid(T));
    std::shared_lock lock(mutex);

    auto lit = listeners.find(type);
    if (lit != listeners.end()) {
        for (void* listener : lit->second) {
            static_cast<MultiEventListenerBase<T>*>(listener)->observe(event);
        }
    }
}

template<typename... TEvents>
void MultiEventDispatcher::unsubscribe(MultiEventListener<TEvents...>* listener) {
    std::type_index types[]  = {std::type_index(typeid(TEvents))...};
    void* const pointers[]   = {static_cast<MultiEventListenerBase<TEvents>*>(listener)...};
    constexpr std::size_t nt = sizeof...(TEvents);

    std::unique_lock lock(mutex);
    for (std::size_t i = 0; i < nt; ++i) { remove(types[i], pointers[i]); }
}

} // namespace bl

#endif

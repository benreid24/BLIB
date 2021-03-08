#ifndef BLIB_EVENTS_DISPATCHERSCOPEGUARD_HPP
#define BLIB_EVENTS_DISPATCHERSCOPEGUARD_HPP

#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace event
{
/**
 * @brief Scope guard for Dispatcher. Listeners that are subscribed through the guard will
 *        be unsubscribed when it goes out of scope
 *
 * @ingroup Events
 *
 */
class DispatcherScopeGuard : private util::NonCopyable {
public:
    /**
     * @brief Create the guard around the given dispatcher
     *
     * @param dispatcher The dispatcher to guard
     */
    DispatcherScopeGuard(Dispatcher& dispatcher);

    /**
     * @brief Unsubscribed any listeners that were subscribed through this guard
     *
     */
    ~DispatcherScopeGuard();

    /**
     * @brief Subscribe the given listener to the event stream for the event types given
     *
     * @tparam TEvents The types of events to subscribe to
     * @param listener The listener to receive events as they are dispatched
     */
    template<typename... TEvents>
    void subscribe(Listener<TEvents...>* listener);

    /**
     * @brief Removes the given listener and prevents it from receiving any more events
     *
     * @tparam TEvents The types of events the listener is subscribed to
     * @param listener The listener to remove
     */
    template<typename... TEvents>
    void unsubscribe(Listener<TEvents...>* listener);

private:
    Dispatcher& dispatcher;
    mutable std::mutex mutex;
    std::unordered_map<std::type_index, std::vector<void*>> listeners;

    void remove(const std::type_index& t, void* val);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TEvents>
void DispatcherScopeGuard::subscribe(Listener<TEvents...>* listener) {
    std::type_index types[]  = {std::type_index(typeid(TEvents))...};
    void* const pointers[]   = {static_cast<ListenerBase<TEvents>*>(listener)...};
    constexpr std::size_t nt = sizeof...(TEvents);

    mutex.lock();
    for (std::size_t i = 0; i < nt; ++i) {
        auto lit = listeners.find(types[i]);
        if (lit == listeners.end()) { lit = listeners.try_emplace(types[i]).first; }
        lit->second.emplace_back(pointers[i]);
    }
    mutex.unlock();

    dispatcher.subscribe(listener);
}

template<typename... TEvents>
void DispatcherScopeGuard::unsubscribe(Listener<TEvents...>* listener) {
    std::type_index types[]  = {std::type_index(typeid(TEvents))...};
    void* const pointers[]   = {static_cast<ListenerBase<TEvents>*>(listener)...};
    constexpr std::size_t nt = sizeof...(TEvents);

    mutex.lock();
    for (std::size_t i = 0; i < nt; ++i) { remove(types[i], pointers[i]); }
    mutex.unlock();

    dispatcher.unsubscribe(listener);
}

} // namespace event
} // namespace bl

#endif

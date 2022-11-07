#ifndef BLIB_EVENTS_DISPATCHER_HPP
#define BLIB_EVENTS_DISPATCHER_HPP

#include <BLIB/Events/Listener.hpp>
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
    void subscribe(Listener<TEvents...>* listener, bool defer = false);

    /**
     * @brief Removes the given listener and prevents it from receiving any more events
     *
     * @tparam TEvents The types of events the listener is subscribed to
     * @param listener The listener to remove
     * @param defer True to defer adding until syncListeners() is called. May receive extra events
     *              but can be called from within event listener observe() methods
     */
    template<typename... TEvents>
    void unsubscribe(Listener<TEvents...>* listener, bool defer = false);

    /**
     * @brief Dispatches the given event to each listener that is subscribed to that type of event
     *
     * @tparam T The type of event to dispatch
     * @param event The event to dispatch
     */
    template<typename T>
    void dispatch(const T& event) const;

    /**
     * @brief Synchronizes the listener buses. Called once per update cycle by the engine
     *
     */
    void syncListeners();

private:
    mutable util::ReadWriteLock readWriteLock;
    std::unordered_map<std::type_index, std::vector<void*>> listeners;
    std::unordered_map<std::type_index, std::unordered_set<void*>> listenerDedup;

    std::mutex addLock;
    std::unordered_map<std::type_index, std::vector<void*>> toAdd;
    std::unordered_map<std::type_index, std::vector<void*>> toRemove;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TEvents>
void Dispatcher::subscribe(Listener<TEvents...>* listener, bool defer) {
    std::type_index types[]  = {std::type_index(typeid(TEvents))...};
    void* const pointers[]   = {static_cast<ListenerBase<TEvents>*>(listener)...};
    constexpr std::size_t nt = sizeof...(TEvents);

    if (defer) {
        std::lock_guard lock(addLock);
        for (std::size_t i = 0; i < nt; ++i) {
            auto lit = toAdd.find(types[i]);
            if (lit == toAdd.end()) { lit = toAdd.try_emplace(types[i]).first; }
            lit->second.emplace_back(pointers[i]);
        }
    }
    else {
        util::ReadWriteLock::WriteScopeGuard lock(readWriteLock);
        for (std::size_t i = 0; i < nt; ++i) {
            auto lit = listeners.find(types[i]);
            if (lit == toAdd.end()) { lit = listeners.try_emplace(types[i]).first; }
            auto sit = listenerDedup.find(types[i]);
            if (sit == listenerDedup.end()) { sit = listenerDedup.try_emplace(types[i]).first; }
            if (sit->second.find(pointers[i]) == sit->second.end()) {
                lit->second.emplace_back(pointers[i]);
                sit->second.emplace(pointers[i]);
            }
        }
    }
}

template<typename T>
void Dispatcher::dispatch(const T& event) const {
    const std::type_index type(typeid(T));
    util::ReadWriteLock::ReadScopeGuard lock(readWriteLock);

    auto lit = listeners.find(type);
    if (lit != listeners.end()) {
        for (void* listener : lit->second) {
            static_cast<ListenerBase<T>*>(listener)->observe(event);
        }
    }
}

template<typename... TEvents>
void Dispatcher::unsubscribe(Listener<TEvents...>* listener, bool defer) {
    std::type_index types[]  = {std::type_index(typeid(TEvents))...};
    void* const pointers[]   = {static_cast<ListenerBase<TEvents>*>(listener)...};
    constexpr std::size_t nt = sizeof...(TEvents);

    if (defer) {
        std::lock_guard lock(addLock);
        for (std::size_t i = 0; i < nt; ++i) {
            auto lit = toRemove.find(types[i]);
            if (lit == toRemove.end()) { lit = toRemove.try_emplace(types[i]).first; }
            lit->second.emplace_back(pointers[i]);
        }
    }
    else {
        for (std::size_t i = 0; i < nt; ++i) {
            auto lit = listeners.find(types[i]);
            if (lit == listeners.end()) { continue; }
            auto sit = listenerDedup.find(types[i]);
            if (sit == listenerDedup.end()) { continue; }
            sit->second.erase(pointers[i]);
            for (unsigned int j = 0; j < lit->second.size(); ++j) {
                if (lit->second[j] == pointers[i]) {
                    lit->second.erase(lit->second.begin() + j);
                    --j;
                }
            }
        }
    }
}

} // namespace event
} // namespace bl

#endif

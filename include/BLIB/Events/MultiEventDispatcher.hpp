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

class MultiEventDispatcher : private NonCopyable {
public:
    template<typename... TEvents>
    void subscribe(MultiEventListener<TEvents...>* listener);

    template<typename... TEvents>
    void unsubscribe(MultiEventListener<TEvents...>* listener);

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

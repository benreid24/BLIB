#ifndef BLIB_EVENTS_SUBSCRIBEHELPERS_HPP
#define BLIB_EVENTS_SUBSCRIBEHELPERS_HPP

#include <BLIB/Events/SingleDispatcher.hpp>

namespace bl
{
namespace event
{
namespace priv
{
template<typename T>
struct SubscriberBase {
    SubscriberBase(SingleDispatcher<T>& bus, ListenerBase<T>* l, bool defer) {
        bus.addListener(l, defer);
    }
};

template<typename... Ts>
struct SubscriberHelper : public SubscriberBase<Ts>... {
    SubscriberHelper(std::vector<SingleDispatcherBase*>& dlist, std::mutex& dlock,
                     Listener<Ts...>* l, bool defer)
    : SubscriberBase<Ts>(SingleDispatcher<Ts>::get(dlist, dlock), static_cast<ListenerBase<Ts>*>(l),
                         defer)... {}
};

template<typename T>
struct UnSubscriberBase {
    UnSubscriberBase(SingleDispatcher<T>& bus, ListenerBase<T>* l, bool defer) {
        bus.removeListener(l, defer);
    }
};

template<typename... Ts>
struct UnSubscriberHelper : public UnSubscriberBase<Ts>... {
    UnSubscriberHelper(std::vector<SingleDispatcherBase*>& dlist, std::mutex& dlock,
                       Listener<Ts...>* l, bool defer)
    : UnSubscriberBase<Ts>(SingleDispatcher<Ts>::get(dlist, dlock),
                           static_cast<ListenerBase<Ts>*>(l), defer)... {}
};

} // namespace priv
} // namespace event
} // namespace bl

#endif

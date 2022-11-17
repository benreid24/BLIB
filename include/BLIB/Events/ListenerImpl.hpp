#ifndef BLIB_EVENTS_LISTENERIMPL_HPP
#define BLIB_EVENTS_LISTENERIMPL_HPP

#ifndef BLIB_EVENTS_DISPATCHER_HPP
#error "Implementation file included out of order. Do not manually include"
#endif

namespace bl
{
namespace event
{
template<typename... TEvents>
Listener<TEvents...>::Listener()
: subscribed(false) {}

template<typename... TEvents>
Listener<TEvents...>::~Listener() {
    if (subscribed) { Dispatcher::unsubscribe(this); }
}

} // namespace event
} // namespace bl

#endif

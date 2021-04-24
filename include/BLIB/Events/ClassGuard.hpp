#ifndef BLIB_EVENTS_CLASSGUARD_HPP
#define BLIB_EVENTS_CLASSGUARD_HPP

#include <BLIB/Events/Dispatcher.hpp>

namespace bl
{
namespace event
{
/**
 * @brief Version of scope guard but for classes. Classes that subscribe to event buses through a
 *        member ClassGuard will automatically unsubscribe when they are destructed. The catch is
 *        that they may only subscribe to a single event bus at a time
 *
 * @tparam TEvents The types of events to subscribe to
 * @ingroup Events
 */
template<typename... TEvents>
class ClassGuard {
public:
    /**
     * @brief Creates an empty guard
     *
     * @param owner The owning class of this guard
     *
     */
    ClassGuard(Listener<TEvents...>* owner);

    /**
     * @brief Unsubscribes if subscribed
     *
     */
    ~ClassGuard();

    /**
     * @brief Subscribes the owner to the given bus
     *
     * @param bus Event bus to subscribe to
     */
    void subscribe(Dispatcher& bus);

    /**
     * @brief Unsubscribes the owner from the given event bus
     *
     */
    void unsubscribe();

private:
    Listener<TEvents...>* const listener;
    Dispatcher* dispatcher;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TEvents>
ClassGuard<TEvents...>::ClassGuard(Listener<TEvents...>* owner)
: listener(owner)
, dispatcher(nullptr) {}

template<typename... TEvents>
ClassGuard<TEvents...>::~ClassGuard() {
    unsubscribe();
}

template<typename... TEvents>
void ClassGuard<TEvents...>::subscribe(Dispatcher& bus) {
    unsubscribe();
    dispatcher = &bus;
    bus.subscribe(listener);
}

template<typename... TEvents>
void ClassGuard<TEvents...>::unsubscribe() {
    if (dispatcher) {
        dispatcher->unsubscribe(listener);
        dispatcher = nullptr;
    }
}

} // namespace event
} // namespace bl

#endif

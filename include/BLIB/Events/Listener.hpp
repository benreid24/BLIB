#ifndef BLIB_EVENTS_LISTENER_HPP
#define BLIB_EVENTS_LISTENER_HPP

namespace bl
{
namespace event
{
/**
 * @brief Base class for Listener. Do not use directly
 * @see Listener
 *
 * @tparam T The type of event to be base for
 * @ingroup Events
 */
template<typename T>
class ListenerBase {
public:
    virtual ~ListenerBase() = default;

    /**
     * @brief Observe the given
     *
     * @param event
     */
    virtual void observe(const T& event) = 0;
};

/**
 * @brief Listener of multiple event types. Listeners should inherit this class with the types of
 *        events they would like to subscribe to as template params. An observe method from
 *        Listener will be created for each passed type and must be implemented
 * @see Dispatcher
 * @see Listener
 *
 * @tparam TEvents The sequence of event types to subscribe to
 * @ingroup Events
 */
template<typename... TEvents>
class Listener : public ListenerBase<TEvents>... {
public:
    virtual ~Listener() = default;
};

} // namespace event
} // namespace bl

#endif

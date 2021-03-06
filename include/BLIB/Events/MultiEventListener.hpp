#ifndef BLIB_EVENTS_MULTIEVENTLISTENER_HPP
#define BLIB_EVENTS_MULTIEVENTLISTENER_HPP

namespace bl
{
/**
 * @brief Base class for MultiEventListener. Do not use directly
 * @see MultiEventListener
 *
 * @tparam T The type of event to be base for
 * @ingroup Events
 */
template<typename T>
class MultiEventListenerBase {
public:
    virtual ~MultiEventListenerBase() = default;

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
 *        MultiEventListenerBase will be created for each passed type and must be implemented
 * @see MultiEventDispatcher
 * @see MultiEventListenerBase
 *
 * @tparam TEvents The sequence of event types to subscribe to
 * @ingroup Events
 */
template<typename... TEvents>
class MultiEventListener : public MultiEventListenerBase<TEvents>... {
public:
    virtual ~MultiEventListener() = default;
};

} // namespace bl

#endif

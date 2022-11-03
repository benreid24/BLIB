#ifndef BLIB_INPUT_LISTENER_HPP
#define BLIB_INPUT_LISTENER_HPP

#include <BLIB/Input/DispatchType.hpp>

namespace bl
{
namespace input
{
class Actor;

/**
 * @brief Base class for input listeners that can subscribe to an Actor. The base class keeps
 *        the input system updated if it gets copied, moved, or destructed
 *
 * @ingroup Input
 *
 */
class Listener {
public:
    /**
     * @brief Construct a new Listener object
     *
     */
    Listener();

    /**
     * @brief Copies from the other listener. If subscribed, replaces the other listener with this
     *        one in the Actor
     *
     * @param copy The listener to replace
     */
    Listener(const Listener& copy);

    /**
     * @brief Copies from the other listener. If subscribed, replaces the other listener with this
     *        one in the Actor
     *
     * @param moved The listener to replace
     */
    Listener(Listener&& moved);

    /**
     * @brief Notifies the listener of an input event. Listeners are notified in the reverse order
     *        of when they subscribed to the Actor
     *
     * @param Actor The input system
     * @param activatedControl The control that was activated, or is activated
     * @param eventType Provides additional context on the control type
     * @param eventTriggered True for controls triggered by user input, false for continuous
     *                       notification of active controls
     * @return True to consume the event and stop the dispatches, false to send to other listeners
     */
    virtual bool observe(const Actor& Actor, unsigned int activatedControl, DispatchType eventType,
                         bool eventTriggered) = 0;

private:
    Actor* owner;

    friend class Actor;
};

} // namespace input
} // namespace bl

#endif

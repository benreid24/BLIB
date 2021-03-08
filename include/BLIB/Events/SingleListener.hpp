#ifndef BLIB_EVENTS_SINGLELISTENER_HPP
#define BLIB_EVENTS_SINGLELISTENER_HPP

#include <SFML/Window.hpp>

namespace bl
{
namespace event
{
/**
 * @brief Templatized utility class for setting up an event dispatcher and listener system
 * @see SingleDispatcher
 *
 * @tparam T The type of event
 * @ingroup Events
 */
template<typename T>
class SingleListener {
public:
    virtual ~SingleListener() = default;

    /**
     * @brief Notification method for the listener to receive events from the dispatcher
     *
     * @param event The event being dispatched
     */
    virtual void observe(const T& event) = 0;
};

} // namespace event
} // namespace bl

#endif

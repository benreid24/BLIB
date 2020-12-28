#ifndef BLIB_UTIL_EVENT_LISTENER_HPP
#define BLIB_UTIL_EVENT_LISTENER_HPP

#include <SFML/Window.hpp>

namespace bg
{
/**
 * @brief Templatized utility class for setting up an event dispatcher and listener system
 * @see EventDispatcher
 *
 * @tparam T The type of event
 * @ingroup Util
 */
template<typename T>
class EventListener {
public:
    virtual ~EventListener() = default;

    /**
     * @brief Notification method for the listener to recieve events from the dispatcher
     *
     * @param event The event being dispatched
     */
    virtual void observe(const T& event) = 0;
};

/// Special instantiation of EventListener for sf::Event window events
typedef EventListener<sf::Event> WindowEventListener;

} // namespace bg

#endif
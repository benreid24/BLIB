#ifndef BLIB_MENU_EVENT_HPP
#define BLIB_MENU_EVENT_HPP

#include <BLIB/Menu/Item.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Data struct representing an event in a Menu
 *
 * @ingroup Menu
 *
 */
struct Event {
    /**
     * @brief The type of event
     *
     */
    enum Type { SelectorMove, Activate, SelectorLocation };

    /**
     * @brief Data struct for a SelectorMove event
     *
     */
    struct MoveEvent {
        Item::AttachPoint direction;
    };

    /**
     * @brief Data struct for an Activate event
     *
     */
    struct ActivateEvent {};

    /**
     * @brief Data struct for a SelectorLocation event
     *
     */
    struct LocationEvent {
        sf::Vector2f position;
    };

    const Type type;
    const union {
        MoveEvent moveEvent;
        ActivateEvent activateEvent;
        LocationEvent locationEvent;
    };

    /**
     * @brief Create a SelectorMove event
     *
     * @param moveEvent The movement information
     */
    Event(const MoveEvent& moveEvent);

    /**
     * @brief Create an Activate event
     *
     * @param activateEvent The activation information
     */
    Event(const ActivateEvent& activateEvent);

    /**
     * @brief Create a SelectorLocation event
     *
     * @param locationEvent The location information
     */
    Event(const LocationEvent& locationEvent);
};

} // namespace menu
} // namespace bl

#endif

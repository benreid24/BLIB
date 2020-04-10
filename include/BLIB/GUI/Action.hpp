#ifndef BLIB_GUI_ACTION_HPP
#define BLIB_GUI_ACTION_HPP

#include <SFML/Window.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief GUI analog of sf::Event. Represents user actions that can propogate through the
 *        Element tree
 *
 * @ingroup GUI
 *
 */
struct Action {
    /**
     * @brief The type of Action that occured
     *
     */
    enum Type {
        MouseEntered,
        MouseLeft,
        Clicked,
        RightClicked,
        MiddleClicked,
        Pressed,
        Released,
        Scrolled,
        KeyPressed,
        KeyReleased,
        TextEntered,
        NUM_ACTIONS
    };

    /**
     * @brief Union containing event data, if any extra data is required
     *
     */
    union TData {
        uint32_t input;        /// Unicode char for TextEntered
        float scroll;          /// Mouse wheel delta for Scrolled
        sf::Keyboard::Key key; /// Key for KeyPressed and KeyReleased
    };

    const Type type;             /// The type of Action
    const TData data;            /// Extra Action data if required by type
    const sf::Vector2f position; /// The mouse position when the Action triggered

    /**
     * @brief Creates an action of the given Type and position
     *
     * @param type The type of Action
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, const sf::Vector2f& pos)
    : type(type)
    , data({.input = 0})
    , position(pos) {}

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param data Character to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, uint32_t c, const sf::Vector2f& pos)
    : type(type)
    , data({.input = c})
    , position(pos) {}

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param data Scroll delta to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, float s, const sf::Vector2f& pos)
    : type(type)
    , data({.scroll = s})
    , position(pos) {}

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param data Key to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, sf::Keyboard::Key key, const sf::Vector2f& pos)
    : type(type)
    , data({.key = key})
    , position(pos) {}
};

} // namespace gui
} // namespace bl

#endif
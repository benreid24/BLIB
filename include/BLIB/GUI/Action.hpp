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
        MouseEntered, /// Mouse moved over Element
        MouseLeft,    /// Mouse moved away from Element
        Clicked, /// Element left clicked
        RightClicked, /// Element right clicked
        Pressed, /// Left mouse button pressed on element
        Dragged, /// Mouse moved while
        Released,
        Scrolled,
        KeyPressed,
        KeyReleased,
        TextEntered,
        NUM_ACTIONS,
        Unknown
    };

    /**
     * @brief Union containing event data, if any extra data is required
     *
     */
    union TData {
        uint32_t input;          /// Unicode char for TextEntered
        float scroll;            /// Mouse wheel delta for Scrolled
        sf::Event::KeyEvent key; /// Key for KeyPressed and KeyReleased
        sf::Vector2f dragStart;  /// Position of mouse when drag started

        TData(uint32_t input);
        TData(float scroll);
        TData(sf::Event::KeyEvent key);
        TData(const sf::Vector2f& drag);
    };

    const Type type;             /// The type of Action
    const TData data;            /// Extra Action data if required by type
    const sf::Vector2f position; /// The mouse position when the Action triggered

    /**
     * @brief Helper method to construct an Action from an SFML event
     *
     */
    static Action fromSFML(const sf::Vector2f& mousePos, const sf::Event& event);

    /**
     * @brief Creates an action of the given Type and position
     *
     * @param type The type of Action
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, const sf::Vector2f& pos)
    : type(type)
    , data(0u)
    , position(pos) {}

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param c Character to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, uint32_t c, const sf::Vector2f& pos)
    : type(type)
    , data(c)
    , position(pos) {}

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param s Scroll delta to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, float s, const sf::Vector2f& pos)
    : type(type)
    , data(s)
    , position(pos) {}

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param key Key to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, sf::Event::KeyEvent key, const sf::Vector2f& pos)
    : type(type)
    , data(key)
    , position(pos) {}

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param drag Start of the drag
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, const sf::Vector2f& drag, const sf::Vector2f& pos)
    : type(type)
    , data(drag)
    , position(pos) {}
};

} // namespace gui
} // namespace bl

#endif
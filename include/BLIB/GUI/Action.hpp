#ifndef BLIB_GUI_ACTION_HPP
#define BLIB_GUI_ACTION_HPP

#include <SFML/Window.hpp>
#include <string>

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
        MouseEntered,          /// Mouse moved over Element
        MouseLeft,             /// Mouse moved away from Element
        LeftClicked,           /// Element left clicked
        RightClicked,          /// Element right clicked
        Pressed,               /// Left mouse button pressed on element
        Dragged,               /// Mouse moved while left mouse button pressed on Element
        Released,              /// Left mouse button released on element
        Scrolled,              /// Mouse wheel scrolled while over Element
        KeyPressed,            /// Keyboard key pressed while Element focused
        KeyReleased,           /// Keyboard key released while Element focused
        TextEntered,           /// Text typed in while Element focused
        Closed,                /// Specific to Window element. Fired when it is closed
        GainedFocus,           /// Element came into focus
        LostFocus,             /// Focus was removed from element
        AcquisitionChanged,    /// Element obtained a new acquisition
        RenderSettingsChanged, /// One of the element's render settings was updated
        Custom,                /// Custom type for custom elements to use
        NUM_ACTIONS,           /// How many valid Action types exist
        Unknown                /// Invalid Action
    };

    /**
     * @brief Generic struct to pack Action with custom data for Custom types
     *
     */
    struct CustomData {
        int subtype;
        int ints[4];
        float floats[4];
        char chars[4];
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
        CustomData custom;       /// Generic data for custom actions

        TData(uint32_t input);
        TData(float scroll);
        TData(sf::Event::KeyEvent key);
        TData(const sf::Vector2f& drag);
        TData(const CustomData& data);
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
     * @brief Makes an empty Action of the given type. For types that have no position or data
     *
     * @param type The type of action to create
     */
    Action(Type type);

    /**
     * @brief Creates an action of the given Type and position
     *
     * @param type The type of Action
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, const sf::Vector2f& pos);

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param c Character to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, uint32_t c, const sf::Vector2f& pos);

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param s Scroll delta to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, float s, const sf::Vector2f& pos);

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param key Key to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, sf::Event::KeyEvent key, const sf::Vector2f& pos);

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Action
     * @param drag Start of the drag
     * @param pos The mouse position relative to the containing window
     */
    Action(Type type, const sf::Vector2f& drag, const sf::Vector2f& pos);

    /**
     * @brief Construct a new custom Action
     *
     * @param data Data for the custom action
     * @param pos The mouse position relative to the containing window
     */
    Action(const CustomData& data, const sf::Vector2f& pos);
};

} // namespace gui
} // namespace bl

#endif
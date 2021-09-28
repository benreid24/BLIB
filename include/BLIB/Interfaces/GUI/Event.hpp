#ifndef BLIB_INTERFACES_GUI_EVENT_HPP
#define BLIB_INTERFACES_GUI_EVENT_HPP

#include <SFML/Window.hpp>
#include <string>
#include <variant>

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
struct Event {
    /**
     * @brief The type of Event that occurred
     *
     */
    enum Type {
        MouseEntered = 0,      /// Mouse moved over Element
        MouseLeft,             /// Mouse moved away from Element
        MouseMoved,            /// Mouse moved while over element
        LeftClicked,           /// Element left clicked
        RightClicked,          /// Element right clicked
        LeftMousePressed,      /// Left mouse button was pressed
        LeftMouseReleased,     /// Left mouse button was released
        RightMousePressed,     /// Right mouse button was pressed
        RightMouseReleased,    /// Right mouse button was released
        Dragged,               /// Mouse moved while left mouse button pressed on Element
        Scrolled,              /// Mouse wheel scrolled while over Element
        KeyPressed,            /// Keyboard key pressed while Element focused
        KeyReleased,           /// Keyboard key released while Element focused
        TextEntered,           /// Text typed in while Element focused
        ValueChanged,          /// Generic action for non text inputs to fire on change
        Moved,                 /// Specific to Window element. Fired when it is moved
        Closed,                /// Specific to Window element. Fired when it is closed
        GainedFocus,           /// Element came into focus
        LostFocus,             /// Focus was removed from element
        AcquisitionChanged,    /// Element obtained a new acquisition
        RenderSettingsChanged, /// One of the element's render settings was updated
        MouseOutsideWindow,    /// The mouse has left the window entirely
        NUM_ACTIONS,           /// How many valid Event types exist
        Unknown                /// Invalid Event
    };

    /**
     * @brief Helper method to construct an Event from an SFML event
     *
     */
    static Event fromSFML(const sf::Event& event, const sf::Vector2f& position = {});

    /**
     * @brief Makes an empty Event of the given type. For types that have no position or data
     *
     * @param type The type of action to create
     */
    Event(Type type);

    /**
     * @brief Special copy constructor that copies an existing event but with a new position
     *
     * @param event The existing event to copy
     * @param position The new position to use
     */
    Event(const Event& event, const sf::Vector2f& position);

    /**
     * @brief Creates an action of the given Type and position
     *
     * @param type The type of Event
     * @param pos The mouse position relative to the containing window
     */
    Event(Type type, const sf::Vector2f& pos);

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Event
     * @param c Character to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Event(Type type, uint32_t c, const sf::Vector2f& pos);

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Event
     * @param s Scroll delta to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Event(Type type, float s, const sf::Vector2f& pos);

    /**
     * @brief Creates an Event of the given Type and float value. No position
     *
     * @param type The type of Event
     * @param v The value to store
     */
    Event(Type type, float v);

    /**
     * @brief Creates an Event of the given Type and bool value. No position
     *
     * @param type The type of Event
     * @param v The value to store
     */
    Event(Type type, bool v);

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Event
     * @param key Key to set the extra data to
     * @param pos The mouse position relative to the containing window
     */
    Event(Type type, sf::Event::KeyEvent key, const sf::Vector2f& pos);

    /**
     * @brief Creates an action of the given Type and position and sets the extra data
     *
     * @param type The type of Event
     * @param drag Start of the drag
     * @param pos The mouse position relative to the containing window
     */
    Event(Type type, const sf::Vector2f& drag, const sf::Vector2f& pos);

    /**
     * @brief Creates an event with a string value
     *
     * @param type The type of Event
     * @param value The string value of the event
     */
    Event(Type type, const std::string& value);

    /**
     * @brief Returns the type of the event
     *
     */
    Type type() const;

    /**
     * @brief Returns the position of the mouse when the event was fired
     *
     */
    const sf::Vector2f& mousePosition() const;

    /**
     * @brief Returns the character entered for text events
     *
     */
    std::uint32_t character() const;

    /**
     * @brief Returns the amount of scroll that occurred
     *
     */
    float scrollDelta() const;

    /**
     * @brief Returns the numeric input value of some event types
     *
     */
    float inputValue() const;

    /**
     * @brief Returns the boolean input value of some event types
     *
     */
    bool toggleValue() const;

    /**
     * @brief Returns the key event data for some event types
     *
     */
    const sf::Event::KeyEvent& key() const;

    /**
     * @brief Returns the starting position of the drag event
     *
     */
    const sf::Vector2f& dragStart() const;

    /**
     * @brief Returns the string value of the event
     *
     */
    const std::string& stringValue() const;

private:
    const Type t;
    std::variant<std::uint32_t, float, bool, sf::Event::KeyEvent, sf::Vector2f, std::string> data;
    const sf::Vector2f position;
};

} // namespace gui
} // namespace bl

#endif
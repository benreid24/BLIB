#ifndef BLIB_GUI_ELEMENTS_ELEMENT_HPP
#define BLIB_GUI_ELEMENTS_ELEMENT_HPP

#include <BLIB/GUI/Action.hpp>
#include <BLIB/GUI/Renderers/RenderSettings.hpp>
#include <BLIB/GUI/Renderers/Renderer.hpp>
#include <BLIB/GUI/Signal.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>

namespace bl
{
namespace gui
{
class Packer;

/**
 * @brief Base class for all GUI elements. Provides the common interface used for positioning,
 *        interaction, and rendering
 *
 * @ingroup GUI
 *
 */
class Element
: public bl::NonCopyable
, private std::enable_shared_from_this<Element> {
public:
    typedef std::shared_ptr<Element> Ptr;
    typedef std::weak_ptr<Element> WPtr;

    /**
     * @brief Destroy the Element object
     *
     */
    virtual ~Element() = default;

    /**
     * @brief Returns the user set id of the Element. This is solely for client use
     *
     */
    const std::string& id() const;

    /**
     * @brief Returns the user set group of the Element. This is solely for client use
     *
     */
    const std::string& group() const;

    /**
     * @brief Sets the requisition of the Element. This is the minimum amount of space it
     *        needs. Pass in (0,0) to reset
     *
     * @param box The size to request. Cannot be smaller than minimumRequisition()
     */
    void setRequisition(const sf::Vector2i& size);

    /**
     * @brief Returns the requisition of the Element
     *
     */
    sf::Vector2i getRequisition() const;

    /**
     * @brief Returns the acquision of the Element. This is the size allocated to it, and mays
     *        be larger or equal to the requisition. The requisition includes the position
     *
     */
    const sf::IntRect& getAcquisition() const;

    /**
     * @brief Returns a modifiable reference to the signal for the given trigger. Undefined
     *        behavior if trigger is not valid
     *
     * @param trigger The signal to get
     * @return Signal& A signal that can be set
     */
    Signal& getSignal(Action::Type trigger);

    /**
     * @brief Returns if the Element is in focus or not
     *
     */
    bool hasFocus() const;

    /**
     * @brief Attempts to take the focus. Returns true if able to take, false otherwise
     *
     */
    bool takeFocus();

    /**
     * @brief If set, takes the focus and forces this Element to be in focus. Returns true if
     *        able to take focus
     *
     * @param force Whether or not to force this Element in focus if others try to takeFocus()
     */
    bool setForceFocus(bool force);

    /**
     * @brief Releases the focus from this Element. Returns true if element not in focus or if
     *        the element is not forcing itself in focus. On a false return other Elements will
     *        not be able to takeFocus(). Only Elements with children need to specialize this
     *        method
     *
     */
    virtual bool releaseFocus();

    /**
     * @brief Brings this Element to the top
     *
     */
    void moveToTop() const;

    /**
     * @brief Returns if the mouse is currently over this Element
     *
     */
    bool mouseOver() const;

    /**
     * @brief Returns if the mouse is over the Element and the Left button is pressed
     *
     */
    bool leftPressed() const;

    /**
     * @brief Returns if the mouse is over the Element and the Right button is pressed
     *
     */
    bool rightPressed() const;

    /**
     * @brief Handles the raw window event. Should be called by parent class
     *
     * @param mousePos Current position of mouse, relative to the window
     * @param event The window event
     * @return True if the event is consumed and no more Elements should be notified
     */
    bool handleEvent(const sf::Vector2f& mousePos, const sf::Event& event);

    /**
     * @brief Processes the Action then calls handleAction() if in focus
     *
     * @param action The action to process
     * @return True if the event is consumed and no more Elements should be notified
     */
    bool processAction(const Action& action);

    /**
     * @brief Marks this Element as requiring a recalculation of acquisition
     *
     */
    void makeDirty();

    /**
     * @brief Returns true if this Element requires a reacquisition
     *
     */
    bool dirty() const;

    /**
     * @brief Removes this Element from its parent. Safe to call at any time
     *
     */
    void remove();

    /**
     * @brief Show or hide the Element. Making it invisible does not change any other element
     *        acquisitions
     *
     * @param visible True to render the Element, false to hide. Default is true
     */
    void setVisible(bool visible);

    /**
     * @brief Returns whether or not the Element is visible
     *
     */
    bool visible() const;

    /**
     * @brief Set whether or not the Element is active and responds to events
     *
     * @param active True to respond to events. Default is true
     */
    void setActive(bool active);

    /**
     * @brief Returns whether or not the Element is active and responds to events
     *
     */
    bool active() const;

    /**
     * @brief Performs any custom logic of the Element
     *
     * @param dt Time elapsed, in seconds, since last update
     */
    virtual void update(float dt) {}

    /**
     * @brief Renders the element using the given renderer
     *
     * @param target The target to render to
     * @param renderer The renderer to use
     */
    void render(sf::RenderTarget& target, Renderer::Ptr renderer) const;

    /**
     * @brief Set the character size. Default is 12. Doesn't apply to all Element types
     *
     * @param size Point size
     */
    void setCharacterSize(unsigned int size);

    /**
     * @brief Set the text fill and outline color. Doesn't apply to all Element types
     *
     * @param fill Fill color of the text
     * @param outline Outline color of the text
     *
     */
    void setColor(sf::Color fill, sf::Color outline);

    /**
     * @brief Set the outline thickness of the text, in pixels. Doesn't apply to all Element
     *        types
     *
     * @param thickness Thickness in pixels
     */
    void setOutlineThickness(unsigned int thickness);

    /**
     * @brief Set the style of the text. See sf::Text::Style. Doesn't apply to all Element
     *        types
     *
     * @param style Style to set
     */
    void setStyle(sf::Uint32 style);

    /**
     * @brief Set the font used by the text. Note that a default font is used if this isn't
     *        called. The default font may be left out of the build by defining BLIB_NO_FONT
     *
     * @param font Resource managed font to use
     */
    void setFont(bl::Resource<sf::Font>::Ref font);

    /**
     * @brief Set the horizontal alignment. Doesn't apply to all Element types
     *
     * @param align The alignment to use
     */
    void setHorizontalAlignment(RenderSettings::Alignment align);

    /**
     * @brief Set the vertical alignment. Doesn't apply to all Element types
     *
     * @param align The alignment to use
     */
    void setVerticalAlignment(RenderSettings::Alignment align);

protected:
    /**
     * @brief Builds a new Element. The group and id are optional and are only used to be
     *        passed to the client callbacks and renderer. They can be used to differentiate
     *        elements for custom rendering or behavior
     *
     * @param group The group the element belongs to
     * @param id The id of this element
     */
    Element(const std::string& group, const std::string& id);

    /**
     * @brief Returns the minimum required size of the Element. Any acquisition smaller than
     *        this size is invalid
     *
     */
    virtual sf::Vector2i minimumRequisition() const = 0;

    /**
     * @brief Sets the acquisition of the Element. Meant to be called by a Container. Clears
     *        the dirty state
     *
     * @param acquisition The area the Element is to occupy
     */
    void assignAcquisition(const sf::IntRect& acquisition);

    /**
     * @brief Special event method called when the Element's acquisition changes. Most elements
     *        need not do anything, but Containers need to repack
     *
     */
    virtual void onAcquisition() {}

    /**
     * @brief The's the parent of the child Element to this Element
     *
     * @param child The child to set the parent to
     */
    void setChildParent(Element::Ptr child);

    /**
     * @brief Bring the given child Element to the top. A child Element calls this on it's
     *        parent when it gains focus. Most elements do not need to implement this except
     *        for Containers
     *
     * @param child The child to move to the top of the screen (rendered last)
     */
    virtual void bringToTop(const Element* child) {}

    /**
     * @brief Removes the child Element. Only implemented by Container
     *
     * @param child The Element to remove. No effect if not present
     */
    virtual void removeChild(const Element* child) {}

    /**
     * @brief Method for child classes to handle raw SFML events. Not recommended to use.
     *        Instead, use handleAction(). This method is called regardless of where the mouse
     *        is and if the Element is in focus. Used by Container.  Note that if this
     *        returns true then handleEvent() will return before performing common processing,
     *        like tracking mouse in/out and clicking
     *
     * @param event The raw event
     * @return True if the event is consumed and no more Elements should be notified
     */
    virtual bool handleRawEvent(const sf::Vector2f& mousePos, const sf::Event& event);

    /**
     * @brief Method for child classes to handle Actions performed on this Element. This method
     *        is only called if the Element is in focus
     *
     * @param action The action to process
     * @return True if the event is consumed and no more Elements should be notified
     */
    virtual void handleAction(const Action& action) {}

    /**
     * @brief Fires the signal that corresponds with the passed Action. This only needs to be
     *        called for Actions specific to derived Elements
     *
     * @param action The action to fire
     */
    void fireSignal(const Action& action);

    /**
     * @brief This method is called when any of the RenderSettings change. Child classes may
     *        need to mark themselves dirty if the new settings cause them to not fit in their
     *        acquisitions
     *
     */
    virtual void settingsChanged(){};

    /**
     * @brief Returns the render settings for this object
     *
     * @return const RenderSettings& The render settings
     */
    const RenderSettings& renderSettings() const;

    /**
     * @brief Actually performs the rendering. This is only called if the element is visible.
     *        Child classes may call specialized methods in the renderer, or implement their
     *        own rendering. The latter is not recommended as then appearance is hard coded
     *
     * @param target The target to render to
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, Renderer::Ptr renderer) const;

    /**
     * @brief Returns the Ptr to this Element
     *
     * @return Element::Ptr Ptr to this Element
     */
    Element::Ptr me();

private:
    const std::string _id;
    const std::string _group;
    RenderSettings settings;
    std::optional<sf::Vector2i> requisition;
    sf::IntRect acquisition;
    Element::WPtr parent;
    Signal signals[Action::NUM_ACTIONS];

    bool _dirty;
    bool _active;
    bool _visible;
    bool isFocused;
    bool focusForced;
    bool isMouseOver;
    bool isLeftPressed;
    bool isRightPressed;
    sf::Vector2f dragStart;

    friend class Packer;
    friend class Renderer;
};

} // namespace gui
} // namespace bl

#endif
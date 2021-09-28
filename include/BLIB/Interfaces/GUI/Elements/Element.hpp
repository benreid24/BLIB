#ifndef BLIB_GUI_ELEMENTS_ELEMENT_HPP
#define BLIB_GUI_ELEMENTS_ELEMENT_HPP

#include <BLIB/Interfaces/GUI/Event.hpp>
#include <BLIB/Interfaces/GUI/Renderers/RenderSettings.hpp>
#include <BLIB/Interfaces/GUI/Renderers/Renderer.hpp>
#include <BLIB/Interfaces/GUI/Signal.hpp>
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
class Slider;

/**
 * @brief Base class for all GUI elements. Provides the common interface used for positioning,
 *        interaction, and rendering. All coordinates and sizes are in the coordinate system
 *        of the parent element
 *
 * @ingroup GUI
 *
 */
class Element
: public bl::util::NonCopyable
, public std::enable_shared_from_this<Element> {
public:
    typedef std::shared_ptr<Element> Ptr;
    typedef std::shared_ptr<const Element> CPtr;
    typedef std::weak_ptr<Element> WPtr;

    /**
     * @brief Destroy the Element object
     *
     */
    virtual ~Element() = default;

    /**
     * @brief Returns a const pointer to the parent element. May be null
     *
     */
    CPtr getParent() const;

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
     * @brief Returns the acquisition of the Element. This is the size allocated to it, and mays
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
    Signal& getSignal(Event::Type trigger);

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
     *        not be able to takeFocus()
     *
     */
    virtual bool releaseFocus();

    /**
     * @brief Removes the focus from ALL elements
     *
     */
    bool clearFocus();

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
     * @brief Handles a GUI event. Typically not called manually
     *
     * @param event The window event
     * @return True if the event is consumed and no more Elements should be notified
     */
    bool processEvent(const Event& event);

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
     * @brief Signifies that this Element should be packed. Takes into account the element
     *        type, visibility, and pack override
     *
     */
    bool packable() const;

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
     * @param markDirty Marks this element dirty if the visibility changed
     */
    void setVisible(bool visible, bool markDirty = true);

    /**
     * @brief Signal to Packers that this element should not be packed. Intended to be used by
     *        advanced Containers that manually pack certain elements
     *
     * @param skip True to skip packing, false to pack normally
     */
    void skipPacking(bool skip);

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
     * @brief Special event handler for scroll events. Default behavior returns false
     *
     * @param scroll The scroll action to process
     * @return True if the event was processed, false if unhandled
     *
     */
    virtual bool handleScroll(const Event& scroll);

    /**
     * @brief Performs any custom logic of the Element
     *
     * @param dt Time elapsed, in seconds, since last update
     */
    virtual void update(float dt);

    /**
     * @brief Renders the element using the given renderer
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    void render(sf::RenderTarget& target, sf::RenderStates states, const Renderer& renderer) const;

    /**
     * @brief Set the character size. Default is 12. Doesn't apply to all Element types
     *
     * @param size Point size
     */
    void setCharacterSize(unsigned int size);

    /**
     * @brief Set the fill and outline color. Doesn't apply to all Element types
     *
     * @param fill Fill color of the element
     * @param outline Outline color of the element
     *
     */
    void setColor(sf::Color fill, sf::Color outline);

    /**
     * @brief Set the outline thickness of the element, in pixels. Doesn't apply to all Element
     *        types
     *
     * @param thickness Thickness in pixels
     */
    void setOutlineThickness(unsigned int thickness);

    /**
     * @brief Set secondary colors of the Element. Doesn't apply to all element types
     *
     * @param fill Secondary fill color of the element
     * @param outline Secondary outline color of the text
     *
     */
    void setSecondaryColor(sf::Color fill, sf::Color outline);

    /**
     * @brief Set the outline thickness of the text, in pixels. Doesn't apply to all Element
     *        types
     *
     * @param thickness Thickness in pixels
     */
    void setSecondaryOutlineThickness(unsigned int thickness);

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
    void setFont(bl::resource::Resource<sf::Font>::Ref font);

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

    /**
     * @brief True if the widget should expand horizontally to fill available space when
     *        packing. If false, the widget will only get its requisition for acquisition
     *
     */
    bool expandsWidth() const;

    /**
     * @brief Set whether or not the widget should expand horizontally when packing
     *
     * @param expand True to fill all space, false to use only requisition
     */
    void setExpandsWidth(bool expand);

    /**
     * @brief True if the widget should expand vertically to fill available space when
     *        packing. If false, the widget will only get its requisition for acquisition
     *
     */
    bool expandsHeight() const;

    /**
     * @brief Set whether or not the widget should expand vertically when packing
     *
     * @param expand True to fill all space, false to use only requisition
     */
    void setExpandsHeight(bool expand);

protected:
    /**
     * @brief Builds a new Element
     *
     */
    Element();

    /**
     * @brief Virtual method for containers to propagate events to their children
     *
     * @param event The event to propagate
     * @return True if a child element handled the event, false if the event should be processed
     */
    virtual bool propagateEvent(const Event& event);

    /**
     * @brief Returns the minimum required size of the Element. Any acquisition smaller than
     *        this size is invalid
     *
     */
    virtual sf::Vector2i minimumRequisition() const = 0;

    /**
     * @brief Returns true. Elements such as window should return false
     *
     */
    virtual bool shouldPack() const;

    /**
     * @brief Marks this element as clean and not needing of re-acquisition
     *
     */
    void markClean();

    /**
     * @brief Sets the parent of the child Element to this Element
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
    virtual void bringToTop(const Element* child);

    /**
     * @brief Removes the child Element. Only implemented by Container
     *
     * @param child The Element to remove. No effect if not present
     */
    virtual void removeChild(const Element* child);

    /**
     * @brief Fires the signal that corresponds with the passed Action. This only needs to be
     *        called for Actions specific to derived Elements
     *
     * @param action The action to fire
     */
    void fireSignal(const Event& action);

    /**
     * @brief Returns the render settings for this object
     *
     * @return const RenderSettings& The render settings
     */
    const RenderSettings& renderSettings() const;

    /**
     * @brief Actually performs the rendering. This is only called if the element is visible.
     *        Child classes may call specialized methods in the renderer, or implement their
     *        own rendering
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const = 0;

    /**
     * @brief Set the acquisition of this element. Meant to be called by a Packer
     *
     * @param acquisition The area to occupy, in local parent coordinates
     */
    void assignAcquisition(const sf::IntRect& acquisition);

    /**
     * @brief Manually set the position of the element. This modifies the acquisition but does
     *        not trigger any signals or repacking
     *
     * @param pos The new position of the element. Relative to the parent element
     */
    void setPosition(const sf::Vector2i& pos);

    /**
     * @brief Returns the Ptr to this Element
     *
     * @return Element::Ptr Ptr to this Element
     */
    Ptr me();

private:
    RenderSettings settings;
    std::optional<sf::Vector2i> requisition;
    sf::IntRect acquisition;
    Element::WPtr parent;
    Signal signals[Event::NUM_ACTIONS];

    bool _dirty;
    bool _active;
    bool _visible;
    bool skipPack;
    bool fillX;
    bool fillY;
    bool isFocused;
    bool focusForced;
    bool isMouseOver;
    bool isLeftPressed;
    bool isRightPressed;
    sf::Vector2f dragStart;

    bool processAction(const Event& action);

    friend class Packer;
    friend class Renderer;
    friend class Slider;
};

} // namespace gui
} // namespace bl

#endif

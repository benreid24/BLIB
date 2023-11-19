#ifndef BLIB_GUI_ELEMENTS_SCROLLAREA_HPP
#define BLIB_GUI_ELEMENTS_SCROLLAREA_HPP

#include <BLIB/Interfaces/GUI/Elements/CompositeElement.hpp>

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <BLIB/Interfaces/GUI/Elements/Slider.hpp>
#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Similar to Box, except scrolling is possible both horizontally and vertically if the
 *        size of the elements packed exceeds the maximum allowed space
 *
 * @ingroup GUI
 * @see Box
 *
 */
class ScrollArea : public CompositeElement<3> {
public:
    typedef std::shared_ptr<ScrollArea> Ptr;

    virtual ~ScrollArea() = default;

    /**
     * @brief Create a new ScrollArea
     *
     * @param packer The packer to use
     * @return Ptr The newly created ScrollArea
     */
    static Ptr create(const Packer::Ptr& packer);

    /**
     * @brief Set the maximum size the scroll area can fill before scrolling is enabled. Note
     *        that if a size is set via setRequisition() then that will take precedence
     *
     * @param size The maximum size. (0,0) to reset
     */
    void setMaxSize(const sf::Vector2f& size);

    /**
     * @brief Set whether or not to always show the vertical scrollbar
     *
     * @param show True to always show, false to hide
     */
    void setAlwaysShowVerticalScrollbar(bool show);

    /**
     * @brief Set whether or not to always show the horizontal scrollbar
     *
     * @param show True to always show, false to hide
     */
    void setAlwaysShowHorizontalScrollbar(bool show);

    /**
     * @brief Set whether or not to never the vertical scrollbar
     *
     * @param neverShow True to never show, false to show when needed
     */
    void setNeverShowVerticalScrollbar(bool neverShow);

    /**
     * @brief Set whether or not to never the horizontal scrollbar
     *
     * @param neverShow True to never show, false to show when needed
     */
    void setNeverShowHorizontalScrollbar(bool neverShow);

    /**
     * @brief Direct way to set the scroll. Parameter is a normalized vector
     *
     * @param scroll The normalized scroll values. 0 is top/left, 1 is bottom/right
     */
    void setScroll(const sf::Vector2f& scroll);

    /**
     * @brief Pack the element into the ScrollArea
     *
     * @param e The element to pack
     */
    void pack(const Element::Ptr& e);

    /**
     * @brief Pack the element into the ScrollArea. Also modifies the Element's expand
     *        properties
     *
     * @param e The element to pack
     * @param fillX True for the element to expand horizontally into all available space
     * @param fillY True for the element to expand vertically into all available space
     */
    void pack(const Element::Ptr& e, bool fillX, bool fillY);

    /**
     * @brief Removes and unpacks all child elements
     *
     * @param immediate True to clear all children immediately, false to wait until update()
     *
     */
    void clearChildren(bool immediate);

    /**
     * @brief Set whether or not to include the scrollbars in the requisition. Default is not to
     *        include them. Not including them allows a tighter requisition, but may result in
     *        scrolling earlier than desired for small boxes
     *
     * @param include True to include, false to exclude.
     */
    void includeScrollbarsInRequisition(bool include);

    /**
     * @brief Returns whether or not this element should receive events that occurred outside the
     *        acquisition of its parent
     *
     * @return True if it should take outside events, false for contained only
     */
    virtual bool receivesOutOfBoundsEvents() const override;

protected:
    /**
     * @brief Create a new ScrollArea
     *
     * @param packer The packer to use
     * @return Ptr The newly created ScrollArea
     */
    ScrollArea(const Packer::Ptr& packer);

    /**
     * @brief Adjusts the size of the visible window and updates the scrollbars
     *
     */
    virtual void onAcquisition() override;

    /**
     * @brief Calculates and returns the minimum required space. Takes into account the maximum
     *        space allowed
     *
     * @return sf::Vector2i
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Creates the visual component for this element
     *
     * @param renderer The renderer to use to create visual Components
     * @return The visual component for this element
     */
    virtual rdr::Component* doPrepareRender(rdr::Renderer& renderer) override;

    /**
     * @brief Does a bounds check and calls Container::handleRawEvent if in bounds
     *
     * @param event The event that fired
     * @return True if the event was consumed, false otherwise
     */
    virtual bool propagateEvent(const Event& event) override;

    /**
     * @brief Scrolls the area and returns true
     *
     * @param scroll The scroll that occurred
     * @return True if the scroll was over the area, false otherwise
     */
    virtual bool handleScroll(const Event& scroll) override;

private:
    Slider::Ptr horScrollbar;
    Slider::Ptr vertScrollbar;
    Box::Ptr contentWrapper;
    Box::Ptr content;
    std::optional<sf::Vector2f> maxSize;
    mutable sf::Vector2f totalSize;
    mutable sf::Vector2f availableSize;
    sf::Vector2f offset;
    sf::Vector2f boxMousePos;
    bool alwaysShowH;
    bool alwaysShowV;
    bool neverShowH;
    bool neverShowV;
    bool includeBars;

    void refreshSize() const;
    void scrolled();
    void updateContentPos();
};
} // namespace gui
} // namespace bl

#endif

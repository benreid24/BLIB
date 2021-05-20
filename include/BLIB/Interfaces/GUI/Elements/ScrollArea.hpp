#ifndef BLIB_GUI_ELEMENTS_SCROLLAREA_HPP
#define BLIB_GUI_ELEMENTS_SCROLLAREA_HPP

#include <BLIB/Interfaces/GUI/Elements/Container.hpp>

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
class ScrollArea : public Container {
public:
    typedef std::shared_ptr<ScrollArea> Ptr;

    virtual ~ScrollArea() = default;

    /**
     * @brief Create a new ScrollArea
     *
     * @param packer The packer to use
     * @param group The group of the ScrollArea
     * @param id The id of this ScrollArea
     * @return Ptr The newly created ScrollArea
     */
    static Ptr create(Packer::Ptr packer, const std::string& group = "",
                      const std::string& id = "");

    /**
     * @brief Set the maximum size the scroll area can fill before scrolling is enabled. Note
     *        that if a size is set via setAcquisition() then that will take precedence
     *
     * @param size The maximum size. (0,0) to reset
     */
    void setMaxSize(const sf::Vector2i& size);

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
     * @brief Pack the element into the ScrollArea
     *
     * @param e The element to pack
     */
    void pack(Element::Ptr e);

    /**
     * @brief Pack the element into the ScrollArea. Also modifies the Element's expand
     *        properties
     *
     * @param e The element to pack
     * @param fillX True for the element to expand horizontally into all available space
     * @param fillY True for the element to expand vertically into all available space
     */
    void pack(Element::Ptr e, bool fillX, bool fillY);

protected:
    /**
     * @brief Create a new ScrollArea
     *
     * @param packer The packer to use
     * @param group The group of the ScrollArea
     * @param id The id of this ScrollArea
     * @return Ptr The newly created ScrollArea
     */
    ScrollArea(Packer::Ptr packer, const std::string& group, const std::string& id);

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
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Returns the scroll offset if e is not one of the scrollbars
     *
     * @param e The element to get the offset for
     * @return The offset to apply to events to the element
     */
    virtual sf::Vector2f getElementOffset(const Element* e) const override;

    /**
     * @brief Renders the scroll area, scrollbars if visible, and the child elements
     *
     * @param target  The target to render to
     * @param states States to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

    /// @brief True
    virtual bool consumesScrolls() const override { return true; }

private:
    Packer::Ptr packer;
    Slider::Ptr horScrollbar;
    Slider::Ptr vertScrollbar;
    std::unordered_set<const Element*> filter;
    std::optional<sf::Vector2i> maxSize;
    mutable sf::Vector2i totalSize;
    mutable sf::Vector2i availableSize;
    sf::Vector2f offset;
    bool alwaysShowH;
    bool alwaysShowV;

    void addBars();
    void refreshSize() const;
    void scrolled();
    void mouseScroll(const Action& scroll);
};
} // namespace gui
} // namespace bl

#endif
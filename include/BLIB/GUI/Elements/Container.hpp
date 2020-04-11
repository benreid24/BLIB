#ifndef BLIB_GUI_ELEMENTS_CONTAINER_HPP
#define BLIB_GUI_ELEMENTS_CONTAINER_HPP

#include <BLIB/GUI/Elements/Element.hpp>
#include <BLIB/GUI/Packers/Packer.hpp>
#include <list>

namespace bl
{
namespace gui
{
/**
 * @brief Case class for container type Elements, such as Box, ScrollArea, or Window
 *
 * @ingroup GUI
 *
 */
class Container : public Element {
public:
    typedef std::shared_ptr<Container> Ptr;

    /**
     * @brief Construct a new Container with the given id, group, and Packer
     *
     * @param group The group of the Element
     * @param id The id of this Element
     * @param packer The Packer to pack child elements with
     */
    Container(const std::string& group, const std::string& id, Packer::Ptr packer);

    /**
     * @brief Destroy the Container object
     *
     */
    virtual ~Container() = default;

    /**
     * @brief Releases the focus of this Element, parent, and children, if not forced to stay
     *        in focus
     * @see Element::releaseFocus()
     */
    virtual bool releaseFocus() override;

    /**
     * @brief Performs the removal of Elements pending removal. Then updates all children
     *
     */
    virtual void update(float dt) override;

    /**
     * @brief Renders the container and all of its children in bottom up Z order
     *
     * @param target The target to render to
     * @param renderer The renderer to use
     */
    virtual void render(sf::RenderTarget& target, Renderer::Ptr renderer) const override;

protected:
    /**
     * @brief Returns the minimum requisition for the Container. This depends on the child
     *        elements and the Packer
     *
     * @return sf::Vector2f Minimum size required by the Container
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Repacks elements with the new acquisition
     *
     */
    virtual void onAcquisition() override;

    /**
     * @brief Brings the child Element on top
     *
     * @param child The element to raise
     */
    virtual void bringToTop(const Element* child) override;

    /**
     * @brief Removes the given Element from the list of child Elements
     *
     * @param child The Element to remove
     */
    virtual void removeChild(const Element* child) override;

    /**
     * @brief Passes the event to all child elements, in Z order, and returns true on the first
     *        Element that consumes the event, or false if none consume
     *
     * @param mousePos Position of the mouse when the event fired
     * @param event The event that fired
     * @return True if the event was consumed, false otherwise
     */
    virtual bool handleRawEvent(const sf::Vector2f& mousePos, const sf::Event& event) override;

private:
    Packer::Ptr packer;
    std::vector<Element::Ptr> children;
    std::list<const Element*> toRemove;
};

} // namespace gui
} // namespace bl

#endif
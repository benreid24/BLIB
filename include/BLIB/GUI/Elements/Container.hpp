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
     * @param packer The Packer to pack child elements with
     * @param group The group of the Element
     * @param id The id of this Element
     */
    Container(Packer::Ptr packer, const std::string& group = "", const std::string& id = "");

    /**
     * @brief Destroy the Container object
     *
     */
    virtual ~Container() = default;

    /**
     * @brief Update the Packer used. Marks the Element as dirty
     *
     */
    void setPacker(Packer::Ptr packer);

    /**
     * @brief Releases the focus of this Element, parent, and children, if not forced to stay
     *        in focus
     * @see Element::releaseFocus()
     */
    virtual bool releaseFocus() override;

    /**
     * @brief Adds the Element as a new child. Causes a refresh of acquisitions
     *
     * @param child The child to add
     */
    void add(Element::Ptr child);

    /**
     * @brief Performs the removal of Elements pending removal. Then updates all children
     *
     */
    virtual void update(float dt) override;

protected:
    /**
     * @brief Returns the minimum requisition for the Container. This depends on the child
     *        elements and the Packer
     *
     * @return sf::Vector2i Minimum size required by the Container
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Repacks elements with the new acquisition
     *
     */
    virtual void onAcquisition() override;

    /**
     * @brief Raises the child Element to the front of the rendering/update queue. Only meant
     *        to be called by GUI
     *
     * @param child The child to bring to the top
     */
    void raiseChild(const Element* child);

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

    /**
     * @brief Renders the container and all of its children in bottom up Z order
     *
     * @param target The target to render to
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, Renderer::Ptr renderer) const override;

private:
    Packer::Ptr packer;
    std::vector<Element::Ptr> children;
    std::list<const Element*> toRemove;
};

} // namespace gui
} // namespace bl

#endif
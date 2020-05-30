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
    static Ptr create(Packer::Ptr packer, const std::string& group = "",
                      const std::string& id = "");

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
     * @brief Adds the Element as a new child. Causes a refresh of acquisitions
     *        Sets the expand properties of the child element
     *
     * @param child The child to add and modify
     * @param expandX Whether or not the element should expand horizontally when packing
     * @param expandY Whether or not the element should expand vertically when packing
     */
    void add(Element::Ptr child, bool expandX, bool expandY);

    /**
     * @brief Performs the removal of Elements pending removal. Then updates all children
     *
     */
    virtual void update(float dt) override;

protected:
    /**
     * @brief Construct a new Container with the given id, group, and Packer
     *
     * @param packer The Packer to pack child elements with
     * @param group The group of the Element
     * @param id The id of this Element
     */
    Container(Packer::Ptr packer, const std::string& group = "", const std::string& id = "");

    /**
     * @brief Returns the minimum requisition for the Container. This depends on the child
     *        elements and the Packer
     *
     * @return sf::Vector2i Minimum size required by the Container
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief If true child elements are packed when the acquisition changes. Set to false to
     *        manually manage acquisitions and packing from derived classes. Note that if this
     *        is false then derived classes must manually repack when marked dirty
     *
     */
    bool& autopack();

    /**
     * @brief Packs the child elements into the given acquisition. Must be called from
     *        onAcquisition if it is overriden
     *
     * @param acquisition The acquisition to pack into, relative to the parent
     */
    void packChildren(const sf::IntRect& acquisition);

    /**
     * @brief Raises the child Element to the front of the rendering/update queue
     *
     * @param child The child to bring to the top
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
     * @param event The event that fired
     * @return True if the event was consumed, false otherwise
     */
    virtual bool handleRawEvent(const RawEvent& event) override;

    /**
     * @brief Renders the container and all of its children in bottom up Z order
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          Renderer::Ptr renderer) const override;

    /**
     * @brief Utility method to render the child elements. Allows specialized containers to
     *        implement their own rendering
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    void renderChildren(sf::RenderTarget& target, sf::RenderStates states,
                        Renderer::Ptr renderer) const;

private:
    bool shouldPack;
    mutable sf::RenderTexture renderTexture;
    Packer::Ptr packer;
    std::vector<Element::Ptr> packableChildren;
    std::vector<Element::Ptr> nonpackableChildren;
    std::vector<Element::Ptr> children;
    std::list<const Element*> toRemove;

    /**
     * @brief Packs children elements into the acquisition assigned. Derived classes may
     *        disable autopack() to manage their own acquisitions and packing using
     *        packChildren()
     *
     */
    void onAcquisition();
};

} // namespace gui
} // namespace bl

#endif
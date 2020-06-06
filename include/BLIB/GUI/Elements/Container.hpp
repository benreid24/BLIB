#ifndef BLIB_GUI_ELEMENTS_CONTAINER_HPP
#define BLIB_GUI_ELEMENTS_CONTAINER_HPP

#include <BLIB/GUI/Elements/Element.hpp>
#include <list>
#include <unordered_set>

namespace bl
{
namespace gui
{
/**
 * @brief Base class for Elements that have child elements. Provides coordinate transofmation
 *        for rendering and events, as well as event propogation
 *
 * @ingroup GUI
 *
 */
class Container : public Element {
public:
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
     * @brief Performs the removal of Elements pending removal. Then updates all children.
     *        Class overriding this should still call Container::update to update all
     *        children and to remove pending deleted elements
     *
     */
    virtual void update(float dt) override;

protected:
    /**
     * @brief Construct a new Container with the given id and group
     *
     * @param group The group of the Element
     * @param id The id of this Element
     */
    Container(const std::string& group = "", const std::string& id = "");

    /**
     * @brief Adds the Element as a new child. Marks dirty. Note that this cannot be called
     *        from within the constructor of derived classes
     *
     * @param child The child to add
     */
    void add(Element::Ptr child);

    /**
     * @brief Returns a non mutable list of the child elements that are packable. Each element
     *        is mutable
     *
     * @return const std::vector<Element::Ptr>& A creference to the list of packable elements
     */
    const std::vector<Element::Ptr>& getPackableChildren() const;

    /**
     * @brief Returns a non mutable list of the child elements that are not packable. Each
     *        element is mutable
     *
     * @return const std::vector<Element::Ptr>& A list of non packable elements
     */
    const std::vector<Element::Ptr>& getNonPackableChildren() const;

    /**
     * @brief Called when the acquisition changes. This method is identical to subscribing to
     *        the AcquisitionChanged signal, but is typically universal for all Containers
     *        so it is provided here for convenience
     *
     */
    virtual void onAcquisition() = 0;

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
     *        Element that consumes the event, or false if none consume. Note that if derived
     *        elements override this they should still call it to propogate the event down
     *
     * @param event The event that fired
     * @return True if the event was consumed, false otherwise
     */
    virtual bool handleRawEvent(const RawEvent& event) override;

    /**
     * @brief This is for sophisticated containers to transform the position of events to local
     *        coordinates for elements that have additional offsets applied beyond only the
     *        position of the Container. ScrollArea uses this for its elements. The default
     *        behavior is to return pos unaltered
     *
     * @param e The element that is receiving the event
     * @return sf::Vector2i The offset to apply to RawEvents
     */
    virtual sf::Vector2f getElementOffset(const Element* e) const;

    /**
     * @brief Utility method to render the child elements. This will call computeView to
     *        constrain rendering and will offset the child elements by the Containers
     *        position. Containers that need to manually render elements should apply the view
     *        from computeView() then use renderChildrenRawFiltered() to filter out those they
     *        need to manually render
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    void renderChildren(sf::RenderTarget& target, sf::RenderStates states,
                        Renderer::Ptr renderer) const;

    /**
     * @brief Renders the children of this Container excluding those in filter. Does not apply
     *       any additional translations or apply any view. That must be done manually
     *
     * @param target Target to render to. View should be set to computeView()
     * @param states Render states to use. Transform must include translation to global coords
     * @param renderer The renderer to use
     * @param filter Set of elements to skip while rendering
     */
    void renderChildrenRawFiltered(sf::RenderTarget& target, sf::RenderStates states,
                                   Renderer::Ptr renderer,
                                   const std::unordered_set<const Element*>& filter) const;

    /**
     * @brief Computes a View that can be used for constrained rendering of child elements into
     *        the acquisition with no spill over
     *
     * @param target The target being rendered to
     * @param transform The transform to apply that was passed down into render()
     * @param region The region to render into. Defaults to the acquisition
     * @return sf::View A View that can be applied to the target to constrain rendering
     */
    sf::View computeView(sf::RenderTarget& target, const sf::Transform& transform,
                         sf::IntRect region = {}) const;

private:
    std::vector<Element::Ptr> packableChildren;
    std::vector<Element::Ptr> nonpackableChildren;
    std::vector<Element::Ptr> children;
    std::list<const Element*> toRemove;

    void acquisitionCb();
};

} // namespace gui
} // namespace bl

#endif
#ifndef BLIB_INTERFACES_GUI_ELEMENTS_COMPOSITEELEMENT_HPP
#define BLIB_INTERFACES_GUI_ELEMENTS_COMPOSITEELEMENT_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Intermediate class for elements that consist of other elements. Provides some helper
 *        methods to ensure contained elements receive events and position updates
 *
 * @tparam N The number of contained elements
 */
template<std::size_t N>
class CompositeElement : public Element {
public:
    /**
     * @brief Destroy the Composite Element object
     *
     */
    virtual ~CompositeElement() = default;

    /**
     * @brief Updates the child elements
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override;

    /**
     * @brief Releases the focus of this Element, parent, and children, if not forced to stay
     *        in focus
     *
     * @param requester The element requesting for focus to be cleared
     *
     */
    virtual bool releaseFocus(const Element* requester) override;

protected:
    /**
     * @brief Construct a new Composite Element
     *
     */
    CompositeElement();

    /**
     * @brief Initializes the internal child element pointers
     *
     * @param children Array of pointers to child elements
     */
    void registerChildren(Element** children);

    /**
     * @brief Called when the acquisition changes. Allows elements to easily position children
     *
     */
    virtual void onAcquisition() = 0;

    /**
     * @brief Does not affect child, but requests to move this element to the top as well
     *
     * @param child Ignored
     */
    virtual void bringToTop(const Element* child) override;

    /**
     * @brief Sends the given event to each child (in order of registerChildren) and returns the
     *        result. Optionally sends the event to all children or terminates once one consumes
     *
     * @param event The event to send
     * @param stopWhenConsumed True to stop sending when consumed, false to send to all
     * @return True if a child consumed the event, false otherwise
     */
    bool sendEventToChildren(const Event& event, bool stopWhenConsumed = true);

    /**
     * @brief Calls prepareRender on each child
     *
     * @param renderer The GUI renderer instance
     */
    virtual void prepareChildrenRender(rdr::Renderer& renderer) override;

    /**
     * @brief Returns whether or not this element should receive events that occurred outside the
     *        acquisition of its parent
     *
     * @return True if it should take outside events, false for contained only
     */
    virtual bool receivesOutOfBoundsEvents() const override;

    /**
     * @brief Adds all children to the current scene
     *
     * @param renderer The GUI renderer instance
     */
    virtual void addChildrenToScene(rdr::Renderer& renderer) override;

private:
    Element* children[N];

    void moveCb();
    void acquisitionCb();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<std::size_t N>
CompositeElement<N>::CompositeElement()
: Element() {
    getSignal(Event::Moved).willAlwaysCall(std::bind(&CompositeElement<N>::moveCb, this));
    getSignal(Event::AcquisitionChanged)
        .willAlwaysCall(std::bind(&CompositeElement<N>::acquisitionCb, this));
}

template<std::size_t N>
void CompositeElement<N>::update(float dt) {
    Element::update(dt);
    if (dirty()) { assignAcquisition(getAcquisition()); }
    for (std::size_t i = 0; i < N; ++i) { children[i]->update(dt); }
}

template<std::size_t N>
void CompositeElement<N>::registerChildren(Element** c) {
    for (std::size_t i = 0; i < N; ++i) {
        children[i] = c[i];
        setChildParent(children[i]);
    }
}

template<std::size_t N>
bool CompositeElement<N>::sendEventToChildren(const Event& e, bool s) {
    bool r = false;
    for (std::size_t i = 0; i < N; ++i) {
        if (children[i]->processEvent(e)) {
            if (s) return true;
            r = true;
        }
    }
    return r;
}

template<std::size_t N>
void CompositeElement<N>::bringToTop(const Element*) {
    moveToTop();
}

template<std::size_t N>
void CompositeElement<N>::prepareChildrenRender(rdr::Renderer& renderer) {
    for (std::size_t i = 0; i < N; ++i) { children[i]->prepareRender(renderer); }
}

template<std::size_t N>
void CompositeElement<N>::addChildrenToScene(rdr::Renderer& r) {
    for (std::size_t i = 0; i < N; ++i) { children[i]->addToScene(r); }
}

template<std::size_t N>
void CompositeElement<N>::moveCb() {
    for (std::size_t i = 0; i < N; ++i) { children[i]->recalculatePosition(); }
}

template<std::size_t N>
void CompositeElement<N>::acquisitionCb() {
    onAcquisition();
}

template<std::size_t N>
bool CompositeElement<N>::releaseFocus(const Element* r) {
    if (!Element::releaseFocus(r)) return false;

    for (unsigned int i = 0; i < N; ++i) {
        if (!children[i]->releaseFocus(r)) return false;
    }
    return true;
}

template<std::size_t N>
bool CompositeElement<N>::receivesOutOfBoundsEvents() const {
    for (unsigned int i = 0; i < N; ++i) {
        if (children[i]->receivesOutOfBoundsEvents()) return true;
    }
    return false;
}

} // namespace gui
} // namespace bl

#endif

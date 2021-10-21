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
     * @brief Sends the given event to each child (in order of registerChildren) and returns the
     *        reuslt. Optionally sends the event to all children or terminates once one consumes
     *
     * @param event The event to send
     * @param stopWhenConsumed True to stop sending when consumed, false to send to all
     * @return True if a child consumed the event, false otherwise
     */
    bool sendEventToChildren(const Event& event, bool stopWhenConsumed = true);

    /**
     * @brief Renders each child element in the order received in registerChildren
     *
     * @param target The target to render to
     * @param states The render states to use
     * @param renderer The renderer to use
     * @param changeView True to compute a new view, false to use the current
     */
    void renderChildren(sf::RenderTarget& target, sf::RenderStates states, const Renderer& renderer,
                        bool changeView) const;

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
void CompositeElement<N>::renderChildren(sf::RenderTarget& target, sf::RenderStates states,
                                         const Renderer& renderer, bool changeView) const {
    const sf::View oldView = target.getView();
    if (changeView) {
        target.setView(
            interface::ViewUtil::computeSubView(getAcquisition(), renderer.getOriginalView()));
    }

    for (std::size_t i = 0; i < N; ++i) { children[i]->render(target, states, renderer); }

    target.setView(oldView);
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

} // namespace gui
} // namespace bl

#endif

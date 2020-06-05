#include <BLIB/GUI/Elements/Container.hpp>

namespace bl
{
namespace gui
{
namespace
{
bool deleteElement(std::vector<Element::Ptr>& list, const Element* e) {
    bool del = false;
    for (unsigned int i = 0; i < list.size(); ++i) {
        if (list[i].get() == e) {
            list.erase(list.begin() + i);
            --i;
            del = true;
        }
    }
    return del;
}

RawEvent makeFakeMove() {
    sf::Event sfevent;
    sfevent.type        = sf::Event::MouseMoved;
    sfevent.mouseMove.x = -100000;
    sfevent.mouseMove.y = -100000;
    return RawEvent(sfevent, {-100000, -100000}, sf::Transform::Identity);
}

template<typename T>
sf::Rect<T> intersection(const sf::Rect<T>& r, const sf::Rect<T>& l) {
    const sf::Rect<T> rc(r.left, r.top, r.left + r.width, r.top + r.height);
    const sf::Rect<T> lc(l.left, l.top, l.left + l.width, l.top + l.height);
    const sf::Rect<T> c(std::max(rc.left, lc.left),
                        std::max(rc.top, lc.top),
                        std::min(rc.width, lc.width),
                        std::min(rc.height, lc.height));
    return {c.left, c.top, c.width - c.left, c.height - c.top};
}
} // namespace

Container::Container(const std::string& group, const std::string& id)
: Element(group, id) {
    getSignal(Action::AcquisitionChanged)
        .willAlwaysCall(std::bind(&Container::acquisitionCb, this));
}

bool Container::releaseFocus() {
    for (Element::Ptr e : children) {
        if (!e->releaseFocus()) return false;
    }
    return Element::releaseFocus();
}

void Container::acquisitionCb() { onAcquisition(); }

void Container::bringToTop(const Element* child) {
    for (unsigned int i = 1; i < nonpackableChildren.size(); ++i) {
        if (nonpackableChildren[i].get() == child) {
            Element::Ptr c = nonpackableChildren[i];
            nonpackableChildren.erase(nonpackableChildren.begin() + i);
            nonpackableChildren.insert(nonpackableChildren.begin(), c);
            return;
        }
    }
}

void Container::add(Element::Ptr e) {
    children.insert(children.begin(), e);
    if (e->packable())
        packableChildren.push_back(e);
    else
        nonpackableChildren.push_back(e);
    setChildParent(e);
    makeDirty();
}

const std::vector<Element::Ptr>& Container::getPackableChildren() const {
    return packableChildren;
}

const std::vector<Element::Ptr>& Container::getNonPackableChildren() const {
    return nonpackableChildren;
}

void Container::removeChild(const Element* child) { toRemove.push_back(child); }

bool Container::handleRawEvent(const RawEvent& event) {
    static const RawEvent fakeMove = makeFakeMove();
    bool sendFakes                 = false;
    const RawEvent transformed     = event.transformToLocal(
        {static_cast<float>(getAcquisition().left), static_cast<float>(getAcquisition().top)});

    for (Element::Ptr e : nonpackableChildren) {
        if (sendFakes)
            e->handleEvent(fakeMove);
        else if (e->handleEvent(transformed))
            sendFakes = true;
    }
    for (Element::Ptr e : packableChildren) {
        if (sendFakes)
            e->handleEvent(fakeMove);
        else if (e->handleEvent(transformed))
            sendFakes = true;
    }

    // allow Element::handleEvent to complete for this element now if sendFakes is false
    return sendFakes;
}

void Container::update(float dt) {
    if (!toRemove.empty()) makeDirty();
    for (const Element* e : toRemove) {
        deleteElement(nonpackableChildren, e);
        deleteElement(packableChildren, e);
        deleteElement(children, e);
    }
    toRemove.clear();
    if (dirty()) {
        assignAcquisition(getAcquisition());
        markClean();
    }
    for (Element::Ptr e : children) { e->update(dt); }
}

void Container::renderChildren(sf::RenderTarget& target, sf::RenderStates states,
                               Renderer::Ptr renderer) const {
    // Save old view
    const sf::View oldView = target.getView();

    // Cast
    const sf::FloatRect acq = static_cast<sf::FloatRect>(getAcquisition());
    const float w           = static_cast<float>(target.getSize().x);
    const float h           = static_cast<float>(target.getSize().y);

    // Compute region children will be drawn in and constrain
    const sf::FloatRect oldRegion(oldView.getCenter() - oldView.getSize() / 2.f,
                                  oldView.getSize());
    const sf::FloatRect region = states.transform.transformRect(acq);
    const sf::FloatRect constrained(intersection(oldRegion, region));
    sf::View view(constrained);

    // Compute viewport and constrain
    const sf::FloatRect port(constrained.left / w,
                             constrained.top / h,
                             constrained.width / w,
                             constrained.height / h);
    view.setViewport(intersection(port, oldView.getViewport()));
    if (view.getViewport().width < 0 || view.getViewport().height < 0) {
        // Restore view
        target.setView(oldView);
        return;
    }
    target.setView(view);

    // Transform children
    states.transform.translate(static_cast<float>(getAcquisition().left),
                               static_cast<float>(getAcquisition().top));

    // Draw children
    for (auto it = packableChildren.rbegin(); it != packableChildren.rend(); ++it) {
        (*it)->render(target, states, renderer);
    }
    for (auto it = nonpackableChildren.rbegin(); it != nonpackableChildren.rend(); ++it) {
        (*it)->render(target, states, renderer);
    }

    // Restore view
    target.setView(oldView);
}

} // namespace gui
} // namespace bl
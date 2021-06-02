#include <BLIB/Interfaces/GUI/Elements/Container.hpp>

#include <BLIB/Interfaces/Utilities.hpp>

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

bool viewValid(const sf::View& v) {
    const sf::FloatRect& a = v.getViewport();
    if (a.width < 0.f) return false;
    if (a.height < 0.f) return false;
    if (a.left < 0.f) return false;
    if (a.top < 0.f) return false;
    return true;
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
    moveToTop();
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

const std::vector<Element::Ptr>& Container::getPackableChildren() const { return packableChildren; }

const std::vector<Element::Ptr>& Container::getNonPackableChildren() const {
    return nonpackableChildren;
}

void Container::removeChild(const Element* child) { toRemove.push_back(child); }

RawEvent Container::transformEvent(const RawEvent& e) const {
    return e.transformToLocal(
        {static_cast<float>(getAcquisition().left), static_cast<float>(getAcquisition().top)});
}

bool Container::handleRawEvent(const RawEvent& event) {
    static const RawEvent fakeMove = makeFakeMove();
    bool sendFakes                 = false;
    const RawEvent transformed     = transformEvent(event);

    for (Element::Ptr e : nonpackableChildren) {
        if (sendFakes)
            e->handleEvent(fakeMove);
        else if (e->handleEvent(transformed.transformToLocal(getElementOffset(e.get()))))
            sendFakes = true;
    }
    for (Element::Ptr e : packableChildren) {
        if (sendFakes)
            e->handleEvent(fakeMove);
        else if (e->handleEvent(transformed.transformToLocal(getElementOffset(e.get()))) &&
                 (event.event.type != sf::Event::MouseWheelScrolled || e->consumesScrolls()))
            sendFakes = true;
    }

    // allow Element::handleEvent to complete for this element now if sendFakes is false
    return sendFakes;
}

sf::Vector2f Container::getElementOffset(const Element* e) const { return {0, 0}; }

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
                               const Renderer& renderer) const {
    // Save old view
    const sf::View oldView = target.getView();

    // Compute new view
    const sf::View view = computeView(target);
    if (!viewValid(view)) {
        // Restore view
        target.setView(oldView);
        return;
    }
    target.setView(view);

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

void Container::renderChildrenRawFiltered(sf::RenderTarget& target, sf::RenderStates states,
                                          const Renderer& renderer,
                                          const std::unordered_set<const Element*>& filter) const {
    for (auto it = packableChildren.rbegin(); it != packableChildren.rend(); ++it) {
        if (filter.find(it->get()) == filter.end()) (*it)->render(target, states, renderer);
    }
    for (auto it = nonpackableChildren.rbegin(); it != nonpackableChildren.rend(); ++it) {
        if (filter.find(it->get()) == filter.end()) (*it)->render(target, states, renderer);
    }
}

sf::View Container::computeView(sf::RenderTarget& target, sf::IntRect area, bool constrain) const {
    if (area.left == 0 && area.top == 0 && area.height == 0 && area.width == 0)
        area = getAcquisition();

    const sf::View oldView = target.getView();

    // Cast
    const sf::FloatRect acq = static_cast<sf::FloatRect>(area);
    const float w           = oldView.getSize().x;
    const float h           = oldView.getSize().y;

    const sf::FloatRect port(acq.left / w, acq.top / h, acq.width / w, acq.height / h);
    sf::View view = interface::ViewUtil::computeView({acq.width, acq.height}, oldView, port);

    if (constrain) {
        const sf::FloatRect cport = intersection(view.getViewport(), oldView.getViewport());
        const float nw            = acq.width * (cport.width / view.getViewport().width);
        const float nh            = acq.height * (cport.height / view.getViewport().height);

        view.setCenter(nw * 0.5f, nh * 0.5f);
        view.setViewport(cport);
        view.setSize(nw, nh);
    }

    return view;
}

} // namespace gui
} // namespace bl
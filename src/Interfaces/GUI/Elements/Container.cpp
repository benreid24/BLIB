#include <BLIB/Interfaces/GUI/Elements/Container.hpp>

#include <BLIB/Interfaces/GUI/Elements/ScrollArea.hpp>
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

bool viewValid(const sf::View& v) {
    const sf::FloatRect& a = v.getViewport();
    if (a.width < 0.f) return false;
    if (a.height < 0.f) return false;
    if (a.left < 0.f) return false;
    if (a.top < 0.f) return false;
    if (a.width > 1.f) return false;
    if (a.height > 1.f) return false;
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
    if (!dynamic_cast<ScrollArea*>(e.get())) {
        e->getSignal(Action::Scrolled).willAlwaysCall([this](const Action& a, Element*) {
            fireSignal(a);
        });
    }
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
    if (!getAcquisition().contains(static_cast<sf::Vector2i>(event.localMousePos))) {
        if (event.event.type != sf::Event::MouseMoved) return false;
    }

    bool sendFakes             = false;
    const RawEvent transformed = transformEvent(event);
    const RawEvent fakeMove =
        event.event.type == sf::Event::MouseMoved ? transformed : makeFakeMove();

    for (Element::Ptr e : nonpackableChildren) {
        if (sendFakes) { e->handleEvent(fakeMove); }
        else if (e->handleEvent(transformed.transformToLocal(getElementOffset(e.get())))) {
            sendFakes = true;
        }
    }
    for (Element::Ptr e : packableChildren) {
        if (sendFakes) { e->handleEvent(fakeMove); }
        else if (e->handleEvent(transformed.transformToLocal(getElementOffset(e.get())))) {
            sendFakes = true;
        }
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
    const sf::View view = computeView(oldView, getAcquisition());
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

sf::View Container::computeView(const sf::View& oldView, const sf::IntRect& area,
                                bool constrain) const {
    const sf::Vector2f oldCorner = oldView.getCenter() - oldView.getSize() * 0.5f;

    const sf::FloatRect acq = static_cast<sf::FloatRect>(area);
    const float w           = oldView.getSize().x;
    const float h           = oldView.getSize().y;

    const sf::FloatRect port(
        (acq.left - oldCorner.x) / w, (acq.top - oldCorner.y) / h, acq.width / w, acq.height / h);
    sf::View view = interface::ViewUtil::computeView({acq.width, acq.height}, oldView, port);

    if (constrain) { constrainView(view, oldView); }

    return view;
}

void Container::constrainView(sf::View& view, const sf::View& oldView) const {
    const sf::FloatRect& newPort = view.getViewport();
    const sf::FloatRect& oldPort = oldView.getViewport();
    const sf::Vector2f m(view.getSize().x / newPort.width, view.getSize().y / newPort.height);
    const float oldBottom = oldPort.top + oldPort.height;
    const float oldRight  = oldPort.left + oldPort.width;

    if (newPort.left < oldPort.left) {
        const float d = oldPort.left - newPort.left;
        const float s = d * m.x;
        view.setSize(view.getSize() - sf::Vector2f(s, 0.f));
        view.move(s * 0.5f, 0.f);
        view.setViewport({oldPort.left, newPort.top, newPort.width - d, newPort.height});
    }

    const float newRight = newPort.left + newPort.width;
    if (newRight > oldRight) {
        const float d = newRight - oldRight;
        const float s = d * m.x;
        view.setSize(view.getSize() - sf::Vector2f(s, 0.f));
        view.move(-s * 0.5f, 0.f);
        view.setViewport({newPort.left, newPort.top, newPort.width - d, newPort.height});
    }

    if (newPort.top < oldPort.top) {
        const float d = oldPort.top - newPort.top;
        const float s = d * m.y;
        view.setSize(view.getSize() - sf::Vector2f(0.f, s));
        view.move(0.f, s * 0.5f);
        view.setViewport({newPort.left, oldPort.top, newPort.width, newPort.height - d});
    }

    const float newBottom = newPort.top + newPort.height;
    if (newBottom > oldBottom) {
        const float d = newBottom - oldBottom;
        const float s = d * m.y;
        view.setSize(view.getSize() - sf::Vector2f(0.f, s));
        view.move(0.f, -s * 0.5f);
        view.setViewport({newPort.left, newPort.top, newPort.width, newPort.height - d});
    }
}

} // namespace gui
} // namespace bl

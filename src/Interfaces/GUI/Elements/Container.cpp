#include <BLIB/Interfaces/GUI/Elements/Container.hpp>

#include <BLIB/Interfaces.hpp>
#include <BLIB/Interfaces/GUI/Elements/ScrollArea.hpp>
#include <BLIB/Interfaces/Utilities.hpp>

namespace bl
{
namespace gui
{
namespace
{
void deleteElement(std::vector<Element::Ptr>& list, const Element* e) {
    for (unsigned int i = 0; i < list.size(); ++i) {
        if (list[i].get() == e) {
            list.erase(list.begin() + i);
            --i;
        }
    }
}

void deleteElement(std::vector<Element*>& list, const Element* e) {
    for (unsigned int i = 0; i < list.size(); ++i) {
        if (list[i] == e) {
            list.erase(list.begin() + i);
            --i;
        }
    }
}

} // namespace

Container::Container()
: Element()
, renderer(nullptr)
, clearFlag(false) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Container::acquisitionCb, this));
    getSignal(Event::Moved).willAlwaysCall(std::bind(&Container::moveCb, this));
}

bool Container::releaseFocus(const Element* requester) {
    if (!Element::releaseFocus(requester)) return false;

    for (Element::Ptr& e : children) {
        if (!e->releaseFocus(requester)) return false;
    }
    return true;
}

void Container::acquisitionCb() { onAcquisition(); }

void Container::moveCb() {
    for (Element* c : zorder) { c->recalculatePosition(); }
}

void Container::bringToTop(const Element* child) {
    for (unsigned int i = 1; i < zorder.size(); ++i) {
        if (zorder[i] == child) {
            Element* c = zorder[i];
            zorder.erase(zorder.begin() + i);
            zorder.insert(zorder.begin(), c);
            assignDepths();
            return;
        }
    }

    moveToTop();
}

void Container::add(const Element::Ptr& e) {
    children.emplace_back(e);
    zorder.insert(zorder.begin(), e.get());
    setChildParent(e.get());
    if (renderer) { e->prepareRender(*renderer); }
    makeDirty();
    assignDepths();
}

const std::vector<Element::Ptr>& Container::getChildren() const { return children; }

void Container::removeChild(const Element* child) { toRemove.push_back(child); }

void Container::clearChildren(bool immediate) {
    clearFlag = true;
    if (immediate) update(0.f);
}

bool Container::propagateEvent(const Event& event) {
    for (unsigned int i = 0; i < zorder.size(); ++i) {
        if (zorder[i]->processEvent(event)) {
            if (event.type() == Event::MouseMoved) {
                const Event fakeMove(Event::MouseMoved, sf::Vector2f(-100000.f, -100000.f));
                for (++i; i < zorder.size(); ++i) { zorder[i]->processEvent(fakeMove); }
            }
            return true;
        }
    }

    return false;
}

bool Container::handleScroll(const Event& event) {
    if (!getAcquisition().contains(event.mousePosition())) return false;

    for (Element* e : zorder) {
        if (e->handleScroll(event)) { return true; }
    }

    return false;
}

void Container::update(float dt) {
    Element::update(dt);

    const bool soiled = clearFlag || !toRemove.empty();
    if (clearFlag) {
        clearFlag = false;
        if (renderer) {
            for (const auto& e : children) { renderer->destroyComponent(*e); }
        }
        children.clear();
        zorder.clear();
    }
    else if (!toRemove.empty()) {
        for (const Element* e : toRemove) {
            if (renderer) { renderer->destroyComponent(*e); }
            deleteElement(children, e);
            deleteElement(zorder, e);
        }
        assignDepths();
        toRemove.clear();
    }

    if (dirty()) { assignAcquisition(getAcquisition()); }
    if (soiled) { makeDirty(); }

    for (Element* e : zorder) { e->update(dt); }
}

bool Container::receivesOutOfBoundsEvents() const {
    for (const auto& c : children) {
        if (c->receivesOutOfBoundsEvents()) return true;
    }
    return false;
}

void Container::prepareChildrenRender(rdr::Renderer& r) {
    renderer = &r;
    for (auto& child : children) { child->prepareRender(r); }
}

void Container::assignDepths() {
    float d = 1.f;
    for (Element* child : zorder) {
        if (child->getComponent()) { child->getComponent()->assignDepth(d); }
        d += 1.f;
    }
}

} // namespace gui
} // namespace bl

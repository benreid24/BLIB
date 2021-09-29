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
, clearFlag(false) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Container::acquisitionCb, this));
    getSignal(Event::Moved).willAlwaysCall(std::bind(&Container::moveCb, this));
}

bool Container::releaseFocus() {
    for (Element::Ptr e : children) {
        if (!e->releaseFocus()) return false;
    }
    return Element::releaseFocus();
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
            return;
        }
    }

    moveToTop();
}

void Container::add(const Element::Ptr& e) {
    children.emplace_back(e);
    zorder.insert(zorder.begin(), e.get());
    setChildParent(e);
    makeDirty();
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
    for (Element* e : zorder) {
        if (e->handleScroll(event)) { return true; }
    }

    return false;
}

void Container::update(float dt) {
    if (!toRemove.empty() || clearFlag) makeDirty();

    if (clearFlag) {
        clearFlag = false;
        children.clear();
    }
    else {
        for (const Element* e : toRemove) {
            deleteElement(children, e);
            deleteElement(zorder, e);
        }
    }
    toRemove.clear();

    if (dirty()) {
        assignAcquisition(getAcquisition());
        markClean();
    }

    for (Element* e : zorder) { e->update(dt); }
}

void Container::renderChildren(sf::RenderTarget& target, sf::RenderStates states,
                               const Renderer& renderer, bool changeView) const {
    // Save old view
    const sf::View oldView = target.getView();

    // Compute new view
    const sf::View view = interface::ViewUtil::computeSubView(sf::FloatRect{getAcquisition()},
                                                              target.getDefaultView());
    if (changeView) { target.setView(view); }

    // Draw children
    for (auto it = zorder.rbegin(); it != zorder.rend(); ++it) {
        (*it)->render(target, states, renderer);
    }

    // Restore view
    target.setView(oldView);
}

} // namespace gui
} // namespace bl

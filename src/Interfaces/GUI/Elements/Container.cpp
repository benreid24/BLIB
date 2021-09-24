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

} // namespace

Container::Container()
: Element()
, clearFlag(false) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Container::acquisitionCb, this));
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
        nonpackableChildren.insert(nonpackableChildren.begin(), e);
    setChildParent(e);
    makeDirty();
}

const std::vector<Element::Ptr>& Container::getPackableChildren() const { return packableChildren; }

const std::vector<Element::Ptr>& Container::getNonPackableChildren() const {
    return nonpackableChildren;
}

void Container::removeChild(const Element* child) { toRemove.push_back(child); }

void Container::clearChildren(bool immediate) {
    clearFlag = true;
    if (immediate) update(0.f);
}

bool Container::propagateEvent(const Event& event) {
    for (Element::Ptr e : nonpackableChildren) {
        if (e->processEvent(event)) {
            // if (event.event.type != sf::Event::MouseMoved) { return true; }
            return true;
        }
    }

    bool processed = false;
    for (Element::Ptr e : packableChildren) {
        if (e->processEvent(event)) {
            // if (event.event.type != sf::Event::MouseMoved) { return true; }
            processed = true;
        }
    }

    return processed;
}

bool Container::handleScroll(const Event& event) {
    for (Element::Ptr e : nonpackableChildren) {
        if (e->handleScroll(event)) { return true; }
    }

    for (Element::Ptr e : packableChildren) {
        if (e->handleScroll(event)) { return true; }
    }

    return false;
}

sf::Vector2f Container::getElementOffset(const Element*) const { return {0, 0}; }

void Container::update(float dt) {
    if (!toRemove.empty() || clearFlag) makeDirty();

    if (clearFlag) {
        clearFlag = false;
        children.clear();
        packableChildren.clear();
        nonpackableChildren.clear();
    }
    else {
        for (const Element* e : toRemove) {
            deleteElement(nonpackableChildren, e);
            deleteElement(packableChildren, e);
            deleteElement(children, e);
        }
    }
    toRemove.clear();

    if (dirty()) {
        assignAcquisition(getAcquisition());
        markClean();
    }

    for (Element::Ptr e : children) { e->update(dt); }
}

void Container::renderChildren(sf::RenderTarget& target, sf::RenderStates states,
                               const Renderer& renderer, bool changeView) const {
    // Save old view
    const sf::View oldView = target.getView();

    // Compute new view
    const sf::View view =
        interface::ViewUtil::computeSubView(sf::FloatRect{getAcquisition()}, oldView);
    if (changeView) { target.setView(view); }

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

#include <BLIB/GUI/Elements/Container.hpp>

namespace bl
{
namespace gui
{
Container::Container(const std::string& group, const std::string& id, Packer::Ptr packer)
: Element(group, id)
, packer(packer) {}

bool Container::releaseFocus() {
    for (Element::Ptr e : children) {
        if (!e->releaseFocus()) return false;
    }
    return Element::releaseFocus();
}

sf::Vector2f Container::minimumRequisition() const { return packer->getRequisition(children); }

void Container::onAcquisition() { packer->pack(getAcquisition(), children); }

void Container::bringToTop(const Element* child) {
    for (unsigned int i = 1; i < children.size(); ++i) {
        if (children[i].get() == child) {
            Element::Ptr c = children[i];
            children.erase(children.begin() + i);
            children.insert(children.begin(), c);
            return;
        }
    }
}

void Container::removeChild(const Element* child) { toRemove.push_back(child); }

bool Container::handleRawEvent(const sf::Vector2f& mpos, const sf::Event& event) {
    for (Element::Ptr e : children) {
        if (e->handleEvent(mpos, event)) return true;
    }
    return false; // allows Element::handleEvent to complete for this element now
}

void Container::update(float dt) {
    for (const Element* e : toRemove) {
        for (unsigned int i = 0; i < children.size(); ++i) {
            if (children[i].get() == e) {
                children.erase(children.begin() + i);
                break;
            }
        }
    }
    toRemove.clear();
    for (Element::Ptr e : children) { e->update(dt); }
}

void Container::render(sf::RenderTarget& target, Renderer::Ptr renderer) const {
    renderer->renderContainer(target, *this);
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        (*it)->render(target, renderer);
    }
}

} // namespace gui
} // namespace bl
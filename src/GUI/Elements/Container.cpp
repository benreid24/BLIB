#include <BLIB/GUI/Elements/Container.hpp>

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

Container::Ptr Container::create(Packer::Ptr packer, const std::string& group,
                                 const std::string& id) {
    return Ptr(new Container(packer, group, id));
}

Container::Container(Packer::Ptr packer, const std::string& group, const std::string& id)
: Element(group, id)
, shouldPack(true)
, packer(packer) {
    getSignal(Action::AcquisitionChanged)
        .willAlwaysCall(std::bind(Container::onAcquisition, this));
}

void Container::setPacker(Packer::Ptr p) {
    packer = p;
    makeDirty();
}

bool Container::releaseFocus() {
    for (Element::Ptr e : children) {
        if (!e->releaseFocus()) return false;
    }
    return Element::releaseFocus();
}

sf::Vector2i Container::minimumRequisition() const {
    return packer->getRequisition(packableChildren);
}

bool& Container::autopack() { return shouldPack; }

void Container::onAcquisition() {
    if (autopack()) packChildren(getAcquisition());
}

void Container::packChildren(const sf::IntRect& acq) {
    renderTexture.create(acq.width, acq.height);
    packer->pack({0, 0, acq.width, acq.height}, packableChildren);
}

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

void Container::add(Element::Ptr e) {
    children.insert(children.begin(), e);
    if (e->packable())
        packableChildren.push_back(e);
    else
        nonpackableChildren.push_back(e);
    setChildParent(e);
    makeDirty();
}

void Container::add(Element::Ptr e, bool fx, bool fy) {
    e->setExpandsWidth(fx);
    e->setExpandsHeight(fy);
    add(e);
}

void Container::removeChild(const Element* child) { toRemove.push_back(child); }

bool Container::handleRawEvent(const RawEvent& event) {
    for (Element::Ptr e : children) {
        if (e->handleEvent(event.transformToLocal({static_cast<float>(getAcquisition().left),
                                                   static_cast<float>(getAcquisition().top)})))
            return true;
    }
    return false; // allows Element::handleEvent to complete for this element now
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
        packer->pack({0, 0, getAcquisition().width, getAcquisition().height},
                     packableChildren);
        markClean();
    }
    for (Element::Ptr e : children) { e->update(dt); }
}

void Container::doRender(sf::RenderTarget& target, sf::RenderStates states,
                         Renderer::Ptr renderer) const {
    renderer->renderContainer(target, states, *this);
    renderChildren(target, states, renderer);
}

void Container::renderChildren(sf::RenderTarget& target, sf::RenderStates states,
                               Renderer::Ptr renderer) const {
    renderTexture.clear(sf::Color::Transparent);
    sf::RenderStates childStates = states;
    childStates.transform        = sf::Transform::Identity;
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        (*it)->render(renderTexture, childStates, renderer);
    }
    renderTexture.display();

    sf::Sprite sprite(renderTexture.getTexture());
    sprite.setPosition(getAcquisition().left, getAcquisition().top);
    target.draw(sprite, states);
}

} // namespace gui
} // namespace bl
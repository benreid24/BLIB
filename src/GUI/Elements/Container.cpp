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
    if (renderTexture.getSize().x != getAcquisition().width ||
        renderTexture.getSize().y != getAcquisition().width)
        renderTexture.create(getAcquisition().width, getAcquisition().height);
    if (autopack()) packChildren({0, 0, getAcquisition().width, getAcquisition().height});
}

void Container::makeClean() { onAcquisition(); }

void Container::packChildren(const sf::IntRect& acq) { packer->pack(acq, packableChildren); }

void Container::markForManualPack(Element::Ptr e) {
    if (deleteElement(packableChildren, e.get())) nonpackableChildren.push_back(e);
}

void Container::markForManualRender(Element::Ptr e) { skipRender.insert(e.get()); }

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

void Container::add(Element::Ptr e, bool fx, bool fy) {
    e->setExpandsWidth(fx);
    e->setExpandsHeight(fy);
    add(e);
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
        makeClean();
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
    for (auto it = packableChildren.rbegin(); it != packableChildren.rend(); ++it) {
        if (skipRender.find(it->get()) == skipRender.end())
            (*it)->render(renderTexture, childStates, renderer);
    }
    for (auto it = nonpackableChildren.rbegin(); it != nonpackableChildren.rend(); ++it) {
        if (skipRender.find(it->get()) == skipRender.end())
            (*it)->render(renderTexture, childStates, renderer);
    }
    renderTexture.display();

    sf::Sprite sprite(renderTexture.getTexture());
    sprite.setPosition(getAcquisition().left, getAcquisition().top);
    target.draw(sprite, states);
}

void Container::manuallyRenderChild(Element::Ptr child, sf::RenderTarget& target,
                                    sf::RenderStates states, Renderer::Ptr renderer) const {
    states.transform.translate(getAcquisition().left, getAcquisition().top);
    child->render(target, states, renderer);
}

} // namespace gui
} // namespace bl
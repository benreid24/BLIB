#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
Component::Component(HighlightState hs)
: highlightState(hs)
, enginePtr(nullptr)
, renderer(nullptr)
, owner(nullptr)
, state(UIState::Regular)
, priorAcq{} {}

void Component::setUIState(UIState state) {
    if (highlightState == HighlightState::HighlightedByMouse) {
        renderer->handleComponentState(owner, state);
        notifyUIState(state);
    }
}

void Component::showTooltip() { renderer->displayTooltip(owner); }

void Component::dismissTooltip() { renderer->dismissTooltip(owner); }

void Component::notifyUIState(UIState) {}

void Component::overrideHighlightBehavior(HighlightState behavior) { highlightState = behavior; }

void Component::create(engine::Engine& engine, Renderer& r, Element& o) {
    enginePtr = &engine;
    renderer  = &r;
    owner     = &o;
    parent    = o.getParent() ? o.getParent()->getComponent() : nullptr;

    doCreate(engine, r);

    if (o.active()) {
        if (o.rightPressed() || o.leftPressed()) { setUIState(UIState::Pressed); }
        else if (o.mouseOver()) { setUIState(UIState::Highlighted); }
        else { setUIState(UIState::Regular); }
    }
    else { setUIState(UIState::Disabled); }

    if (parent && parent != this) {
        const ecs::Entity me    = getEntity();
        const ecs::Entity daddy = parent->getEntity();
        engine.ecs().setEntityParent(me, daddy);
    }

    onElementUpdated();
    onRenderSettingChange();
    onAcquisition();
    assignDepth(0.f);
    setVisible(o.visible());
}

void Component::flash() { renderer->flash(owner); }

void Component::onAcquisition() {
    if (owner->getAcquisition() != priorAcq) {
        priorAcq = owner->getAcquisition();
        handleAcquisition();
    }
}

void Component::onMove() {
    const sf::Vector2f priorPos(priorAcq.left, priorAcq.top);
    if (priorPos != owner->getPosition()) {
        priorAcq = owner->getAcquisition();
        handleMove();
    }
}

sf::Vector2f Component::getRequisition() const {
    BL_LOG_ERROR << "Default getRequisition() is being used for element sizing";
    return {60.f, 15.f};
}

void Component::assignDepth(float d) {
    com::Transform2D* transform = enginePtr->ecs().getComponent<com::Transform2D>(getEntity());
    if (transform) { transform->setDepth(d + owner->getDepthBias()); }
    else { BL_LOG_ERROR << "Could not set depth for entity missing transform: " << getEntity(); }
}

void Component::addToScene(rc::Overlay* overlay) {
    Component* parent = owner->getParentComponent();
    // doCreate(*enginePtr, *renderer, parent, *this);

    if (owner->active()) {
        if (owner->rightPressed() || owner->leftPressed()) { setUIState(UIState::Pressed); }
        else if (owner->mouseOver()) { setUIState(UIState::Highlighted); }
        else { setUIState(UIState::Regular); }
    }
    else { setUIState(UIState::Disabled); }

    if (parent && parent != this) {
        const ecs::Entity me    = getEntity();
        const ecs::Entity daddy = parent->getEntity();
        enginePtr->ecs().setEntityParent(me, daddy);
    }

    onElementUpdated();
    onRenderSettingChange();
    onAcquisition();
    assignDepth(0.f);
    setVisible(owner->visible());

    doSceneAdd(overlay);
}

} // namespace rdr
} // namespace gui
} // namespace bl

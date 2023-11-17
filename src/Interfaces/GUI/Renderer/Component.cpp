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
, priorPos(-1.f, -1.f)
, priorWindowPos(-1.f, -1.f)
, priorSize(-1.f, -1.f) {}

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

void Component::create(engine::Engine& engine, Renderer& r, Element& o, Component* parent,
                       Component* windowOrGui) {
    enginePtr = &engine;
    renderer  = &r;
    owner     = &o;

    doCreate(engine, r, parent, windowOrGui ? *windowOrGui : *this);

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
    onAcquisition(o.getLocalPosition(),
                  {o.getAcquisition().left, o.getAcquisition().top}, // TODO - wrong
                  {o.getAcquisition().width, o.getAcquisition().height});
    assignDepth(0.f);
    setVisible(o.visible());
}

void Component::flash() { renderer->flash(owner); }

void Component::onAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f& posFromWindow,
                              const sf::Vector2f& size) {
    if (posFromParent != priorPos || posFromWindow != priorWindowPos || size != priorSize) {
        priorPos       = posFromParent;
        priorWindowPos = posFromWindow;
        priorSize      = size;
        handleAcquisition(priorPos, priorWindowPos, priorSize);
    }
}

void Component::onMove(const sf::Vector2f& posFromParent, const sf::Vector2f& posFromWindow) {
    if (priorPos != posFromParent || priorWindowPos != posFromWindow) {
        priorPos       = posFromParent;
        priorWindowPos = posFromWindow;
        handleMove(priorPos, priorWindowPos);
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

} // namespace rdr
} // namespace gui
} // namespace bl

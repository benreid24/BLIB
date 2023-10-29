#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

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

void Component::create(engine::Engine& engine, Renderer& r, Element& o, Component* windowOrGui) {
    renderer = &r;
    owner    = &o;
    doCreate(engine, r, windowOrGui ? *windowOrGui : *this);
    if (o.active()) {
        if (o.rightPressed() || o.leftPressed()) { setUIState(UIState::Pressed); }
        else if (o.mouseOver()) { setUIState(UIState::Highlighted); }
        else { setUIState(UIState::Regular); }
    }
    else { setUIState(UIState::Disabled); }
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

} // namespace rdr
} // namespace gui
} // namespace bl

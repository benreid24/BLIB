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
, state(UIState::Regular) {}

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

} // namespace rdr
} // namespace gui
} // namespace bl

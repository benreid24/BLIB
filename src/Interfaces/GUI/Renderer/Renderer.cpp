#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

#include <BLIB/Interfaces/GUI/GUI.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
Renderer::Renderer(engine::Engine& engine, GUI& gui, FactoryTable& factory)
: engine(engine)
, gui(gui)
, factory(factory)
, overlay(nullptr)
, flashProvider(factory.createFlashProvider())
, highlightProvider(factory.createHighlightProvider())
, tooltipProvider(factory.createTooltipProvider())
, currentTooltip(nullptr) {}

void Renderer::destroyComponent(const Element& owner) {
    const auto it = components.find(&owner);
    if (it != components.end()) {
        it->second->doSceneRemove();
        components.erase(it);
    }
}

void Renderer::addToOverlay(rc::Overlay* o) {
    removeFromOverlay();
    overlay = o;
    for (auto& pair : components) { pair.second->doSceneAdd(overlay); }
}

void Renderer::removeFromOverlay() {
    if (overlay) {
        for (auto& pair : components) { pair.second->doSceneRemove(); }
        overlay = nullptr;
    }
}

void Renderer::displayTooltip(Element* src) {
    if (src != currentTooltip) {
        currentTooltip = src;
        tooltipProvider->displayTooltip(src, gui.getMousePosition());
    }
}

void Renderer::dismissTooltip(Element* src) {
    if (src == currentTooltip) {
        currentTooltip = nullptr;
        tooltipProvider->dismissTooltip();
    }
}

void Renderer::flash(Element* src) { flashProvider->flashElement(src); }

void Renderer::handleComponentState(Element* src, Component::UIState state) {
    highlightProvider->notifyUIState(src, state);
}

} // namespace rdr
} // namespace gui
} // namespace bl

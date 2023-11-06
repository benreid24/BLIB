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
, currentTooltip(nullptr) {
    flashProvider->doCreate(engine);
    highlightProvider->doCreate(engine);
    tooltipProvider->doCreate(engine);
}

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
    flashProvider->doSceneAdd(o);
    highlightProvider->doSceneAdd(o);
    tooltipProvider->doSceneAdd(o);
}

void Renderer::removeFromOverlay() {
    if (overlay) {
        for (auto& pair : components) { pair.second->doSceneRemove(); }
        overlay = nullptr;
    }
    flashProvider->doSceneRemove();
    highlightProvider->doSceneRemove();
    tooltipProvider->doSceneRemove();
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

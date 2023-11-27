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
, alive(std::make_shared<bool>(true))
, flashProvider(factory.createFlashProvider())
, highlightProvider(factory.createHighlightProvider())
, tooltipProvider(factory.createTooltipProvider())
, currentTooltip(nullptr) {
    flashProvider->doCreate(engine);
    highlightProvider->doCreate(engine);
    tooltipProvider->doCreate(engine);
}

Renderer::~Renderer() { *alive = false; }

void Renderer::destroyComponent(const Element& owner) {
    const auto it = components.find(&owner);
    if (it != components.end()) {
        flashProvider->notifyDestroyed(&owner);
        highlightProvider->notifyDestroyed(&owner);
        tooltipProvider->notifyDestroyed(&owner);
        it->second->doSceneRemove();
        components.erase(it);
    }
}

void Renderer::addToOverlay(rc::Overlay* o) {
    if (overlay) { removeFromOverlay(); }
    overlay = o;
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

void Renderer::update(float dt) {
    flashProvider->update(dt);
    highlightProvider->update(dt);
    tooltipProvider->update(dt);
}

std::shared_ptr<bool> Renderer::getAliveFlag() { return alive; }

void Renderer::addComponentToOverlayIfRequired(Component* com) {
    if (overlay) { com->addToScene(overlay); }
}

void Renderer::removeComponentFromOverlay(Component* com) {
    if (overlay) {
        flashProvider->notifyDestroyed(com->owner);
        highlightProvider->notifyDestroyed(com->owner);
        tooltipProvider->notifyDestroyed(com->owner);
        com->doSceneRemove();
    }
}

} // namespace rdr
} // namespace gui
} // namespace bl

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
Renderer::Renderer(engine::Engine& engine, FactoryTable& factory)
: engine(engine)
, factory(factory)
, overlay(nullptr) {}

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

} // namespace rdr
} // namespace gui
} // namespace bl

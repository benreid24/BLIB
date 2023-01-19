#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
Renderer::Renderer(sf::WindowBase& window)
: state(window)
, textures(state)
, materials(state)
, renderPasses(*this)
, pipelines(*this)
, scenes(*this) {}

Renderer::~Renderer() {
    if (state.device != nullptr) { cleanup(); }
}

void Renderer::cleanup() {
    vkDeviceWaitIdle(state.device);

    // TODO - free textures and materials
    scenes.cleanup();
    pipelines.cleanup();
    renderPasses.cleanup();
    state.cleanup();
    state.device = nullptr;
}

void Renderer::update(float dt) { cameraSystem.update(dt); }

Cameras& Renderer::cameras() { return cameraSystem; }

const Cameras& Renderer::cameras() const { return cameraSystem; }

} // namespace render
} // namespace bl

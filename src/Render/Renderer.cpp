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
, scenes(*this)
, testScene(nullptr) {}

Renderer::~Renderer() {
    if (state.device != nullptr) { cleanup(); }
}

void Renderer::initialize() {
    state.init();
    renderPasses.addDefaults();
    pipelines.createBuiltins();
    testScene = scenes.allocateScene();
}

void Renderer::cleanup() {
    vkDeviceWaitIdle(state.device);

    scenes.cleanup();
    // TODO - free textures and materials
    pipelines.cleanup();
    renderPasses.cleanup();
    state.cleanup();
    state.device = nullptr;
}

void Renderer::update(float dt) { cameraSystem.update(dt); }

void Renderer::renderFrame() {
    SwapRenderFrame* currentFrame = nullptr;
    VkCommandBuffer commandBuffer = nullptr;
    state.beginFrame(currentFrame, commandBuffer);
    // TODO - scene stack and observers
    testScene->recordRenderCommands(*currentFrame, commandBuffer);
    state.completeFrame();
}

} // namespace render
} // namespace bl

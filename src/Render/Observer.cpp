#include <BLIB/Render/Observer.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bl
{
namespace render
{
Observer::Observer(Renderer& r)
: renderer(r)
, resourcesFreed(false) {
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
}

Observer::~Observer() { cleanup(); }

void Observer::cleanup() {
    if (!resourcesFreed) {
        sceneFramebuffers.cleanup([](Framebuffer& fb) { fb.cleanup(); });
        renderFrames.cleanup([](StandardImageBuffer& frame) { frame.destroy(); });
        resourcesFreed = true;
    }
}

void Observer::update(float dt) {
    std::unique_lock lock(mutex);

    if (!cameras.empty()) { cameras.top()->update(dt); }
    if (hasScene()) { scenes.top()->sync(); }
}

Scene* Observer::pushScene() {
    std::unique_lock lock(mutex);

    Scene* s = renderer.scenePool().allocateScene();
    scenes.push(s);
    return s;
}

void Observer::pushScene(Scene* s) {
    std::unique_lock lock(mutex);
    scenes.push(s);
}

Scene* Observer::popSceneNoRelease(bool cam) {
    std::unique_lock lock(mutex);

    Scene* s = scenes.top();
    scenes.pop();
    if (cam) { cameras.pop(); }
    return s;
}

void Observer::popScene(bool cam) {
    std::unique_lock lock(mutex);

    Scene* s = scenes.top();
    scenes.pop();
    renderer.scenePool().destroyScene(s);
    if (cam) { cameras.pop(); }
}

void Observer::clearScenes() {
    std::unique_lock lock(mutex);

    while (!scenes.empty()) {
        Scene* s = scenes.top();
        scenes.pop();
        renderer.scenePool().destroyScene(s);
    }
    while (!cameras.empty()) { cameras.pop(); }
}

void Observer::clearScenesNoRelease() {
    std::unique_lock lock(mutex);

    while (!scenes.empty()) { scenes.pop(); }
    while (!cameras.empty()) { cameras.pop(); }
}

void Observer::popCamera() {
    std::unique_lock lock(mutex);
    cameras.pop();
}

void Observer::clearCameras() {
    std::unique_lock lock(mutex);
    while (!cameras.empty()) { cameras.pop(); }
}

void Observer::renderScene(VkCommandBuffer commandBuffer) {
    std::unique_lock lock(mutex);

    if (hasScene()) {
        const glm::mat4 projView =
            !cameras.empty() ?
                cameras.top()->getProjectionMatrix(viewport) * cameras.top()->getViewMatrix() :
                glm::perspective(75.f, viewport.width / viewport.height, 0.1f, 100.f);
        const VkRect2D renderRegion{{0, 0}, renderFrames.current().bufferSize()};
        const SceneRenderContext ctx(commandBuffer, projView);
        VkClearValue clearColors[1];
        clearColors[0] = {{{0.f, 0.f, 0.f, 1.f}}};

        sceneFramebuffers.current().beginRender(
            commandBuffer, renderRegion, clearColors, std::size(clearColors), true);
        scenes.top()->renderScene(ctx);
        sceneFramebuffers.current().finishRender(commandBuffer);
    }
}

void Observer::compositeSceneWithEffects(VkCommandBuffer commandBuffer) {
    std::unique_lock lock(mutex);

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // TODO - copy over
}

void Observer::assignRegion(const sf::WindowBase& window, unsigned int count, unsigned int i,
                            bool topBottomFirst) {
    std::unique_lock lock(mutex);

    const sf::Vector2u uSize = window.getSize();
    const sf::Vector2f fSize(uSize);
    const sf::Vector2f fHalf(fSize * 0.5f);
    const sf::Vector2u uHalf(fHalf);
    const sf::Vector2i iHalf(uHalf);

    switch (count) {
    case 1:
        scissor.offset  = {0, 0};
        scissor.extent  = {uSize.x, uSize.y};
        viewport.x      = 0.f;
        viewport.y      = 0.f;
        viewport.width  = fSize.x;
        viewport.height = fSize.y;
        break;

    case 2:
        if (topBottomFirst) {
            scissor.extent  = {uSize.x, uHalf.y};
            viewport.width  = fSize.x;
            viewport.height = fHalf.y;
            viewport.x      = 0.f;
            if (i == 0) {
                scissor.offset = {0, 0};
                viewport.y     = 0.f;
            }
            else {
                scissor.offset = {0, iHalf.y};
                viewport.y     = fHalf.y;
            }
        }
        else {
            scissor.extent  = {uHalf.x, uSize.y};
            viewport.width  = fHalf.x;
            viewport.height = fSize.y;
            viewport.y      = 0.f;
            if (i == 0) {
                scissor.offset = {0, 0};
                viewport.x     = 0.f;
            }
            else {
                scissor.offset = {iHalf.x, 0};
                viewport.x     = fHalf.x;
            }
        }
        break;

    case 3:
    case 4:
        scissor.extent  = {uHalf.x, uHalf.y};
        viewport.width  = fHalf.x;
        viewport.height = fHalf.y;

        // top or bottom row
        if (i < 2) { // top row
            viewport.y       = 0.f;
            scissor.offset.y = 0;
        }
        else { // bottom row
            viewport.y       = fHalf.y;
            scissor.offset.y = uHalf.y;
        }

        // left or right column
        if (i % 2 == 0) { // left column
            viewport.x       = 0.f;
            scissor.offset.x = 0;
        }
        else { // right column
            viewport.x       = fHalf.x;
            scissor.offset.x = uHalf.x;
        }
        break;

    default:
        BL_LOG_ERROR << "Invalid observer count: " << count;
        break;
    }

    if (!renderFrames.valid() ||
        (scissor.extent.width != renderFrames.current().bufferSize().width ||
         scissor.extent.height != renderFrames.current().bufferSize().height)) {
        vkDeviceWaitIdle(renderer.vulkanState().device);

        renderFrames.init(renderer.vulkanState(), [this](StandardImageBuffer& frame) {
            frame.create(renderer.vulkanState(), scissor.extent);
        });

        // scene frame buffers
        VkRenderPass scenePass = renderer.renderPassCache()
                                     .getRenderPass(Config::RenderPassIds::OffScreenSceneRender)
                                     .rawPass();
        unsigned int i = 0;
        sceneFramebuffers.init(renderer.vulkanState(), [this, &i, scenePass](Framebuffer& fb) {
            fb.create(renderer.vulkanState(), scenePass, renderFrames.getRaw(i).attachmentSet());
            ++i;
        });
    }
}

void Observer::setDefaultNearFar(float n, float f) {
    defaultNear = n;
    defaultFar  = f;
}

} // namespace render
} // namespace bl

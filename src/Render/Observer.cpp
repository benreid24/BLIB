#include <BLIB/Render/Observer.hpp>

#include <BLIB/Render/Cameras/2D/Camera2D.hpp>
#include <BLIB/Render/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bl
{
namespace gfx
{
Observer::Observer(Renderer& r)
: renderer(r)
, resourcesFreed(false) {
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};

    overlayProjView = overlayCamera.getProjectionMatrix(viewport) * overlayCamera.getViewMatrix();
}

Observer::~Observer() {
    if (renderer.vulkanState().device != nullptr) { cleanup(); }
}

void Observer::cleanup() {
    if (!resourcesFreed) {
        clearScenes();
        defaultPostFX.reset();
        sceneFramebuffers.cleanup([](vk::Framebuffer& fb) { fb.cleanup(); });
        renderFrames.cleanup([](vk::StandardAttachmentBuffers& frame) { frame.destroy(); });
        resourcesFreed = true;
    }
}

void Observer::updateCamera(float dt) {
    if (!scenes.empty()) { scenes.back().camera->update(dt); }
}

void Observer::pushScene(Scene* s) {
    scenes.emplace_back(renderer, s);
    onSceneAdd();
}

Overlay* Observer::createSceneOverlay() {
    if (scenes.empty()) {
        BL_LOG_ERROR << "Tried to create Overlay with no current scene";
        throw std::runtime_error("Tried to create Overlay with no current scene");
    }

    if (scenes.back().overlay) { renderer.scenePool().destroyScene(scenes.back().overlay); }
    scenes.back().overlay      = renderer.scenePool().allocateScene<Overlay>();
    scenes.back().overlayIndex = scenes.back().overlay->registerObserver();
    return scenes.back().overlay;
}

Overlay* Observer::getOrCreateSceneOverlay() {
    if (scenes.empty()) {
        BL_LOG_ERROR << "Tried to create Overlay with no current scene";
        throw std::runtime_error("Tried to create Overlay with no current scene");
    }

    return scenes.back().overlay ? scenes.back().overlay : createSceneOverlay();
}

Overlay* Observer::getCurrentOverlay() { return scenes.empty() ? nullptr : scenes.back().overlay; }

void Observer::setApplyPostFXToOverlay(bool apply) {
    if (!scenes.empty()) { scenes.back().overlayPostFX = apply; }
}

Scene* Observer::popSceneNoRelease() {
    Scene* s = scenes.back().scene;
    if (scenes.back().overlay) { renderer.scenePool().destroyScene(scenes.back().overlay); }
    scenes.pop_back();
    return s;
}

void Observer::popScene() {
    Scene* s = scenes.back().scene;
    renderer.scenePool().destroyScene(s);
    if (scenes.back().overlay) { renderer.scenePool().destroyScene(scenes.back().overlay); }
    scenes.pop_back();
}

void Observer::clearScenes() {
    while (!scenes.empty()) {
        Scene* s = scenes.back().scene;
        if (scenes.back().overlay) { renderer.scenePool().destroyScene(scenes.back().overlay); }
        scenes.pop_back();
        renderer.scenePool().destroyScene(s);
    }
}

void Observer::clearScenesNoRelease() {
    for (auto& scene : scenes) {
        if (scene.overlay) { renderer.scenePool().destroyScene(scene.overlay); }
    }
    scenes.clear();
}

void Observer::onSceneAdd() {
    scenes.back().postfx->bindImages(renderFrames);
    scenes.back().observerIndex = scenes.back().scene->registerObserver();
#if SCENE_DEFAULT_CAMERA == 2
    setCamera<c2d::Camera2D>(
        glm::vec2{viewport.x + viewport.width * 0.5f, viewport.y + viewport.height * 0.5f},
        glm::vec2{viewport.width, viewport.height});
#else
    setCamera<c3d::Camera3D>();
#endif
}

void Observer::removePostFX() { setPostFX<scene::PostFX>(renderer); }

void Observer::handleDescriptorSync() {
    if (hasScene()) {
        const glm::mat4 projView = scenes.back().camera->getProjectionMatrix(viewport) *
                                   scenes.back().camera->getViewMatrix();
        scenes.back().scene->updateObserverCamera(scenes.back().observerIndex, projView);
        scenes.back().scene->handleDescriptorSync();
        if (scenes.back().overlay) {
            scenes.back().overlay->updateObserverCamera(scenes.back().overlayIndex,
                                                        overlayProjView);
            scenes.back().overlay->handleDescriptorSync();
        }
    }
}

void Observer::renderScene(VkCommandBuffer commandBuffer) {
    const VkRect2D renderRegion{{0, 0}, renderFrames.current().bufferSize()};
    sceneFramebuffers.current().beginRender(
        commandBuffer, renderRegion, clearColors, std::size(clearColors), true);

    if (hasScene()) {
#ifdef BLIB_DEBUG
        if (!scenes.back().camera) {
            BL_LOG_ERROR << "Scene pushed to Observer without calling setCamera()";
        }
#endif

        const VkViewport parentViewport = ovy::Viewport::scissorToViewport(renderRegion);
        scene::SceneRenderContext ctx(commandBuffer,
                                      scenes.back().observerIndex,
                                      parentViewport,
                                      Config::RenderPassIds::OffScreenSceneRender,
                                      false);
        scenes.back().scene->renderScene(ctx);

        if (scenes.back().overlay && scenes.back().overlayPostFX) {
            scene::SceneRenderContext ctx(commandBuffer,
                                          scenes.back().overlayIndex,
                                          parentViewport,
                                          Config::RenderPassIds::OffScreenSceneRender,
                                          false);
            scenes.back().overlay->renderScene(ctx);
        }
    }

    sceneFramebuffers.current().finishRender(commandBuffer);
}

void Observer::compositeSceneWithEffects(VkCommandBuffer commandBuffer) {
    scene::PostFX* fx;
    if (!hasScene()) {
        if (!defaultPostFX) { defaultPostFX = std::make_unique<scene::PostFX>(renderer); }
        defaultPostFX->bindImages(renderFrames);
        fx = defaultPostFX.get();
    }
    else { fx = scenes.back().postfx.get(); }

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    fx->compositeScene(commandBuffer);
}

void Observer::renderOverlay(VkCommandBuffer commandBuffer) {
    if (!scenes.empty() && scenes.back().overlay && !scenes.back().overlayPostFX) {
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        scene::SceneRenderContext ctx(commandBuffer,
                                      scenes.back().overlayIndex,
                                      viewport,
                                      Config::RenderPassIds::SwapchainPrimaryRender,
                                      false);
        scenes.back().overlay->renderScene(ctx);
    }
}

void Observer::assignRegion(const sf::Vector2u& windowSize,
                            const sf::Rect<std::uint32_t>& renderRegion, unsigned int count,
                            unsigned int i, bool topBottomFirst) {
    const std::uint32_t offsetX = (windowSize.x - renderRegion.width) / 2;
    const std::uint32_t offsetY = (windowSize.y - renderRegion.height) / 2;
    const sf::Vector2u uSize(renderRegion.getSize());
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

    // add offsets for letterboxing
    viewport.x += offsetX;
    viewport.y += offsetY;
    scissor.offset.x += offsetX;
    scissor.offset.y += offsetY;

    if (!renderFrames.valid() ||
        (scissor.extent.width != renderFrames.current().bufferSize().width ||
         scissor.extent.height != renderFrames.current().bufferSize().height)) {
        vkCheck(vkDeviceWaitIdle(renderer.vulkanState().device));

        renderFrames.init(renderer.vulkanState(), [this](vk::StandardAttachmentBuffers& frame) {
            frame.create(renderer.vulkanState(), scissor.extent);
        });

        // scene frame buffers
        VkRenderPass scenePass = renderer.renderPassCache()
                                     .getRenderPass(Config::RenderPassIds::OffScreenSceneRender)
                                     .rawPass();
        unsigned int i = 0;
        sceneFramebuffers.init(renderer.vulkanState(), [this, &i, scenePass](vk::Framebuffer& fb) {
            fb.create(renderer.vulkanState(), scenePass, renderFrames.getRaw(i).attachmentSet());
            ++i;
        });

        // post fx image descriptors
        for (SceneInstance& scene : scenes) { scene.postfx->bindImages(renderFrames); }
    }
}

void Observer::setDefaultNearFar(float n, float f) {
    defaultNear = n;
    defaultFar  = f;
}

void Observer::setClearColor(const glm::vec4& color) {
    clearColors[0].color = {color.x, color.y, color.z, color.w};
}

} // namespace gfx
} // namespace bl

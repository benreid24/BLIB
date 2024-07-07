#include <BLIB/Render/RenderTarget.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Graph/Assets/SceneAsset.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bl
{
namespace rc
{
RenderTarget::RenderTarget(engine::Engine& e, Renderer& r, rg::AssetFactory& f, bool rt)
: isRenderTexture(rt)
, engine(e)
, renderer(r)
, graphAssets(f, this)
, resourcesFreed(false) {
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};

    overlayProjView = overlayCamera.getProjectionMatrix(viewport) * overlayCamera.getViewMatrix();
}

RenderTarget::~RenderTarget() {
    if (renderer.vulkanState().device != nullptr) { cleanup(); }
}

void RenderTarget::cleanup() {
    if (!resourcesFreed) {
        clearScenes();
        resourcesFreed = true;
    }
}

void RenderTarget::update(float dt) {
    if (!scenes.empty()) {
        if (scenes.back().camera) { scenes.back().camera->update(dt); }
        scenes.back().graph.update(dt);
        overlayCamera.update(dt);
    }
}

void RenderTarget::pushScene(SceneRef s) {
    scenes.emplace_back(engine, renderer, this, graphAssets, s);
    onSceneAdd();
}

Overlay* RenderTarget::createSceneOverlay() {
    if (scenes.empty()) {
        BL_LOG_ERROR << "Tried to create Overlay with no current scene";
        throw std::runtime_error("Tried to create Overlay with no current scene");
    }

    scenes.back().overlayRef   = renderer.scenePool().allocateScene<Overlay>();
    scenes.back().overlay      = static_cast<Overlay*>(scenes.back().overlayRef.get());
    scenes.back().overlayIndex = scenes.back().overlay->registerObserver();
    return scenes.back().overlay;
}

Overlay* RenderTarget::getOrCreateSceneOverlay() {
    if (scenes.empty()) {
        BL_LOG_ERROR << "Tried to create Overlay with no current scene";
        throw std::runtime_error("Tried to create Overlay with no current scene");
    }

    // if the current scene is an overlay then return that
    Overlay* overlay = dynamic_cast<Overlay*>(scenes.back().scene.get());
    if (overlay) { return overlay; }

    return scenes.back().overlay ? scenes.back().overlay : createSceneOverlay();
}

Overlay* RenderTarget::getCurrentOverlay() {
    if (scenes.empty()) { return nullptr; }

    Overlay* overlay = dynamic_cast<Overlay*>(scenes.back().scene.get());
    if (overlay) { return overlay; }

    return scenes.back().overlay;
}

SceneRef RenderTarget::popSceneNoRelease() {
    SceneRef s = scenes.back().scene;
    scenes.pop_back();
    onSceneChange();
    return s;
}

void RenderTarget::popScene() {
    scenes.pop_back();
    onSceneChange();
}

void RenderTarget::removeScene(Scene* scene) {
    if (!scenes.empty() && scenes.back().scene == scene) { popScene(); }
    else {
        std::erase_if(scenes, [scene](const SceneInstance& s) { return s.scene.get() == scene; });
    }
}

void RenderTarget::clearScenes() { scenes.clear(); }

void RenderTarget::onSceneAdd() {
    scenes.back().observerIndex = scenes.back().scene->registerObserver();
    onSceneChange();
}

void RenderTarget::onSceneChange() {
    if (hasScene()) {
        graphAssets.replaceAsset<rgi::SceneAsset>(scenes.back().scene.get());
        scenes.back().graph.populate(renderer.getRenderStrategy(), *scenes.back().scene);
        graphAssets.releaseUnused();
    }
}

void RenderTarget::handleDescriptorSync() {
    if (hasScene()) {
        if (!scenes.back().camera) {
            if (!dynamic_cast<Overlay*>(scenes.back().scene.get())) {
                BL_LOG_WARN
                    << "Scene being rendered before having a camera set. Creating default camera";
            }
            scenes.back().camera = scenes.back().scene->createDefaultCamera();
            scenes.back().scene->setDefaultNearAndFarPlanes(*scenes.back().camera);
        }

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

void RenderTarget::setClearColor(const glm::vec4& color) {
    clearColors[0].color = {{color.x, color.y, color.z, color.w}};
}

glm::vec2 RenderTarget::transformToWorldSpace(const glm::vec2& sp) const {
    const glm::vec2 ndc((sp.x - viewport.x) / viewport.width * 2.f - 1.f,
                        (sp.y - viewport.y) / viewport.height * 2.f - 1.f);
    glm::mat4 tform(1.f);
    if (hasScene() && scenes.back().camera) {
        tform = scenes.back().camera->getProjectionMatrix(viewport) *
                scenes.back().camera->getViewMatrix();
    }
    tform                  = glm::inverse(tform);
    const glm::vec4 result = tform * glm::vec4(ndc, 0.f, 1.f);
    return {result.x, result.y};
}

glm::vec2 RenderTarget::transformToOverlaySpace(const glm::vec2& sp) const {
    const glm::vec2 ndc((sp.x - viewport.x) / viewport.width * 2.f - 1.f,
                        (sp.y - viewport.y) / viewport.height * 2.f - 1.f);
    cam::OverlayCamera& cam = const_cast<cam::OverlayCamera&>(overlayCamera);
    glm::mat4 tform = tform = cam.getProjectionMatrix(viewport) * cam.getViewMatrix();
    tform                   = glm::inverse(tform);
    const glm::vec4 result  = tform * glm::vec4(ndc, 0.f, 1.f);
    return {result.x, result.y};
}

void RenderTarget::setCamera(std::unique_ptr<cam::Camera>&& cam) {
    if (hasScene()) {
        scenes.back().camera = std::move(cam);
        scenes.back().scene->setDefaultNearAndFarPlanes(*scenes.back().camera);
    }
    else { BL_LOG_ERROR << "Tried to set camera for render target with no current scene"; }
}

void RenderTarget::renderScene(VkCommandBuffer commandBuffer) {
    if (hasScene()) {
#ifdef BLIB_DEBUG
        if (!scenes.back().camera) {
            BL_LOG_ERROR << "Scene pushed to RenderTarget without calling setCamera()";
        }
#endif

        if (scenes.back().graph.needsRepopulation()) {
            scenes.back().graph.populate(renderer.getRenderStrategy(), *scenes.back().scene);
        }

        scenes.back().graph.execute(commandBuffer, scenes.back().observerIndex, isRenderTexture);
    }
}

void RenderTarget::compositeSceneAndOverlay(VkCommandBuffer commandBuffer) {
    if (hasScene()) {
        scenes.back().graph.executeFinal(
            commandBuffer, scenes.back().observerIndex, isRenderTexture);

        if (scenes.back().overlay) {
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            VkClearAttachment attachment{};
            attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            attachment.clearValue = clearColors[1];

            VkClearRect rect{};
            rect.rect           = scissor;
            rect.baseArrayLayer = 0;
            rect.layerCount     = 1;

            vkCmdClearAttachments(commandBuffer, 1, &attachment, 1, &rect);

            scene::SceneRenderContext ctx(commandBuffer,
                                          scenes.back().overlayIndex,
                                          viewport,
                                          isRenderTexture ?
                                              Config::RenderPassIds::StandardAttachmentDefault :
                                              Config::RenderPassIds::SwapchainDefault,
                                          isRenderTexture);
            scenes.back().overlay->renderScene(ctx);
        }
    }
}

} // namespace rc
} // namespace bl

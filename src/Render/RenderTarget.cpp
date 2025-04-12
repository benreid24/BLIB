#include <BLIB/Render/RenderTarget.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/SceneAsset.hpp>
#include <BLIB/Render/Graph/Tasks/RenderOverlayTask.hpp>
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
, resourcesFreed(false)
, renderingTo(nullptr) {
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};
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
    graphAssets.replaceAsset<rgi::SceneAsset>(scenes.back().overlay, rg::AssetTags::OverlayInput);
    scenes.back().graph.removeTasks<rgi::RenderOverlayTask>();
    scenes.back().graph.putTask<rgi::RenderOverlayTask>(&scenes.back().overlayIndex);

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
        graphAssets.replaceAsset<rgi::SceneAsset>(scenes.back().scene.get(),
                                                  rg::AssetTags::SceneInput);
        scenes.back().graph.populate(*scenes.back().scene);
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

        scenes.back().scene->updateObserverCamera(
            scenes.back().observerIndex,
            {scenes.back().camera->getProjectionMatrix(viewport),
             scenes.back().camera->getViewMatrix(),
             scenes.back().camera->getObserverPosition()});
        scenes.back().scene->handleDescriptorSync();
        if (scenes.back().overlay) {
            scenes.back().overlay->updateObserverCamera(
                scenes.back().overlayIndex,
                {overlayCamera.getProjectionMatrix(viewport),
                 overlayCamera.getViewMatrix(),
                 glm::vec3()});
            scenes.back().overlay->handleDescriptorSync();
        }
    }
}

void RenderTarget::syncSceneObjects() {
    if (hasScene()) {
        scenes.back().scene->syncObjects();
        if (scenes.back().overlay) { scenes.back().overlay->syncObjects(); }
    }
}

void RenderTarget::setClearColor(const Color& color) {
    clearColors[0].color = {{color.r(), color.g(), color.b(), color.a()}};
    renderingTo->setShouldClearOnRestart(true);
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

glm::vec2 RenderTarget::getMousePosInWorldSpace() const {
    const auto mpos = sf::Mouse::getPosition(engine.window().getSfWindow());
    return transformToWorldSpace({mpos.x, mpos.y});
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

glm::vec2 RenderTarget::getMousePosInOverlaySpace() const {
    const auto mpos = sf::Mouse::getPosition(engine.window().getSfWindow());
    return transformToOverlaySpace({mpos.x, mpos.y});
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
            scenes.back().graph.populate(*scenes.back().scene);
        }

        scenes.back().graph.execute(commandBuffer, scenes.back().observerIndex, isRenderTexture);
    }
}

void RenderTarget::renderSceneFinal(VkCommandBuffer commandBuffer) {
    if (hasScene()) {
        scenes.back().graph.executeFinal(
            commandBuffer, scenes.back().observerIndex, isRenderTexture);
    }
}

void RenderTarget::resetAssets() { graphAssets.startFrame(); }

} // namespace rc
} // namespace bl

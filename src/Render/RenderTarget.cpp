#include <BLIB/Render/RenderTarget.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>
#include <BLIB/Render/Graph/Assets/SceneAsset.hpp>
#include <BLIB/Render/Graph/Tasks/RenderOverlayTask.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <BLIB/Render/ShaderResources/CameraBufferShaderResource.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bl
{
namespace rc
{
RenderTarget::SceneInstance::SceneInstance(engine::Engine& e, Renderer& r, RenderTarget* owner,
                                           rg::AssetPool& pool, SceneRef s)
: scene(s)
, overlay(nullptr)
, graph(e, r, pool, owner, s.get())
, descriptorCache(r.getGlobalShaderResources(), owner->getShaderResources(),
                  s->getShaderResources())
, observerIndex(0)
, overlayIndex(0) {}

RenderTarget::RenderTarget(engine::Engine& e, Renderer& r, rg::AssetFactory& f, bool rt)
: isRenderTexture(rt)
, engine(e)
, renderer(r)
, graphAssets(f, this)
, descriptorFactories(renderer.descriptorFactoryCache())
, shaderResources(e)
, resourcesFreed(false)
, renderingTo(nullptr) {
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};

    graphAssets.putAsset<rgi::DepthBuffer>();
    shaderResources.getShaderInputWithKey(sri::CameraBufferKey, *this);
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
    scenes.back().overlayIndex = scenes.back().overlay->registerObserver(this);
    scenes.back().overlayDescriptorCache.emplace(renderer.getGlobalShaderResources(),
                                                 getShaderResources(),
                                                 scenes.back().overlay->getShaderResources());
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
    scenes.back().observerIndex = scenes.back().scene->registerObserver(this);
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

void RenderTarget::updateDescriptorsAndQueueTransfers() {
    if (hasScene()) {
        auto& currentScene = scenes.back();

        if (!currentScene.camera) {
            if (!dynamic_cast<Overlay*>(currentScene.scene.get())) {
                BL_LOG_WARN
                    << "Scene being rendered before having a camera set. Creating default camera";
            }
            currentScene.camera = currentScene.scene->createDefaultCamera();
            currentScene.scene->setDefaultNearAndFarPlanes(*currentScene.camera);
        }

        if (currentScene.graph.needsRepopulation()) {
            currentScene.graph.populate(*currentScene.scene);
            if (currentScene.overlay) {
                currentScene.graph.putTask<rgi::RenderOverlayTask>(&currentScene.overlayIndex);
            }
        }

        currentScene.scene->updateDescriptorsAndQueueTransfers();
        if (currentScene.overlay) { currentScene.overlay->updateDescriptorsAndQueueTransfers(); }
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
        auto& currentScene = scenes.back();
#ifdef BLIB_DEBUG
        if (!currentScene.camera) {
            BL_LOG_ERROR << "Scene pushed to RenderTarget without calling setCamera()";
        }
#endif

        currentScene.graph.execute(commandBuffer, currentScene.observerIndex, isRenderTexture);
    }
}

void RenderTarget::resetAssets() { graphAssets.startFrame(); }

ds::DescriptorSetInstanceCache* RenderTarget::getDescriptorSetCache(Scene* scene) {
    for (auto& s : scenes) {
        if (s.scene.get() == scene) { return &s.descriptorCache; }
        if (s.overlay && s.overlay == scene && s.overlayDescriptorCache.has_value()) {
            return &s.overlayDescriptorCache.value();
        }
    }
    return nullptr;
}

void RenderTarget::initPipelineInstance(Scene* scene, std::uint32_t pid,
                                        vk::PipelineInstance& instance) {
    vk::Pipeline* pipeline                         = &renderer.pipelineCache().getPipeline(pid);
    ds::DescriptorSetInstanceCache* descriptorSets = getDescriptorSetCache(scene);
    if (!pipeline) {
        BL_LOG_ERROR << "Failed to get pipeline with id: " << pid;
        return;
    }
    if (!descriptorSets) {
        BL_LOG_ERROR
            << "Failed to get descriptor set cache for scene. RenderTarget does not render it";
        return;
    }
    instance.init(pipeline, *descriptorSets);
}

} // namespace rc
} // namespace bl

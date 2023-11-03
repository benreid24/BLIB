#include <BLIB/Render/Observer.hpp>

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
Observer::Observer(engine::Engine& e, Renderer& r, rg::AssetFactory& f, bool c)
: isCommon(c)
, engine(e)
, renderer(r)
, graphAssets(f, this)
, resourcesFreed(false) {
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};

    overlayProjView = overlayCamera.getProjectionMatrix(viewport) * overlayCamera.getViewMatrix();

    swapframeAsset = graphAssets.putAsset<rgi::FinalSwapframeAsset>(
        renderer.getSwapframeBuffers(), viewport, scissor, clearColors, std::size(clearColors));
}

Observer::~Observer() {
    if (renderer.vulkanState().device != nullptr) { cleanup(); }
}

void Observer::cleanup() {
    if (!resourcesFreed) {
        clearScenes();
        resourcesFreed = true;
    }
}

void Observer::update(float dt) {
    if (!scenes.empty()) {
        if (scenes.back().camera) { scenes.back().camera->update(dt); }
        scenes.back().graph.update(dt);
        overlayCamera.update(dt);
    }
}

void Observer::pushScene(SceneRef s) {
    scenes.emplace_back(engine, renderer, this, graphAssets, s);
    onSceneAdd();
}

Overlay* Observer::createSceneOverlay() {
    if (scenes.empty()) {
        BL_LOG_ERROR << "Tried to create Overlay with no current scene";
        throw std::runtime_error("Tried to create Overlay with no current scene");
    }

    scenes.back().overlayRef   = renderer.scenePool().allocateScene<Overlay>();
    scenes.back().overlay      = static_cast<Overlay*>(scenes.back().overlayRef.get());
    scenes.back().overlayIndex = scenes.back().overlay->registerObserver();
    return scenes.back().overlay;
}

Overlay* Observer::getOrCreateSceneOverlay() {
    if (scenes.empty()) {
        BL_LOG_ERROR << "Tried to create Overlay with no current scene";
        throw std::runtime_error("Tried to create Overlay with no current scene");
    }

    // if the current scene is an overlay then return that
    Overlay* overlay = dynamic_cast<Overlay*>(scenes.back().scene.get());
    if (overlay) { return overlay; }

    return scenes.back().overlay ? scenes.back().overlay : createSceneOverlay();
}

Overlay* Observer::getCurrentOverlay() { return scenes.empty() ? nullptr : scenes.back().overlay; }

SceneRef Observer::popSceneNoRelease() {
    SceneRef s = scenes.back().scene;
    scenes.pop_back();
    onSceneChange();
    return s;
}

void Observer::popScene() {
    scenes.pop_back();
    onSceneChange();
}

void Observer::clearScenes() { scenes.clear(); }

void Observer::onSceneAdd() {
    scenes.back().observerIndex = scenes.back().scene->registerObserver();
    onSceneChange();
}

void Observer::onSceneChange() {
    if (hasScene()) {
        graphAssets.replaceAsset<rgi::SceneAsset>(scenes.back().scene.get());
        if (scenes.back().graph.needsRepopulation()) {
            scenes.back().graph.populate(renderer.getRenderStrategy(), *scenes.back().scene);
        }
        graphAssets.releaseUnused();
    }
}

void Observer::handleDescriptorSync() {
    if (hasScene()) {
        if (!scenes.back().camera) {
            if (!dynamic_cast<Overlay*>(scenes.back().scene.get())) {
                BL_LOG_WARN
                    << "Scene being rendered before having a camera set. Creating default camera";
            }
            scenes.back().camera = scenes.back().scene->createDefaultCamera();
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

void Observer::renderScene(VkCommandBuffer commandBuffer) {
    if (hasScene()) {
#ifdef BLIB_DEBUG
        if (!scenes.back().camera) {
            BL_LOG_ERROR << "Scene pushed to Observer without calling setCamera()";
        }
#endif

        if (scenes.back().graph.needsRepopulation()) {
            scenes.back().graph.populate(renderer.getRenderStrategy(), *scenes.back().scene);
        }

        scenes.back().graph.execute(commandBuffer, scenes.back().observerIndex, false);
    }
}

void Observer::compositeSceneAndOverlay(VkCommandBuffer commandBuffer) {
    if (hasScene()) {
        scenes.back().graph.executeFinal(commandBuffer, scenes.back().observerIndex, false);

        if (scenes.back().overlay) {
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            scene::SceneRenderContext ctx(commandBuffer,
                                          scenes.back().overlayIndex,
                                          viewport,
                                          Config::RenderPassIds::SwapchainDefault,
                                          false);
            scenes.back().overlay->renderScene(ctx);
        }
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
    const VkExtent2D oldSize = scissor.extent;

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

    if (scissor.extent.width != oldSize.width || scissor.extent.height != oldSize.height) {
        vkCheck(vkDeviceWaitIdle(renderer.vulkanState().device));
        graphAssets.notifyResize({scissor.extent.width, scissor.extent.height});
    }
}

void Observer::setClearColor(const glm::vec4& color) {
    clearColors[0].color = {{color.x, color.y, color.z, color.w}};
}

glm::vec2 Observer::transformToWorldSpace(const glm::vec2& sp) const {
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

glm::vec2 Observer::transformToOverlaySpace(const glm::vec2& sp) const {
    const glm::vec2 ndc((sp.x - viewport.x) / viewport.width * 2.f - 1.f,
                        (sp.y - viewport.y) / viewport.height * 2.f - 1.f);
    cam::OverlayCamera& cam = const_cast<cam::OverlayCamera&>(overlayCamera);
    glm::mat4 tform = tform = cam.getProjectionMatrix(viewport) * cam.getViewMatrix();
    tform                   = glm::inverse(tform);
    const glm::vec4 result  = tform * glm::vec4(ndc, 0.f, 1.f);
    return {result.x, result.y};
}

} // namespace rc
} // namespace bl

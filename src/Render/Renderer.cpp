#include <BLIB/Render/Renderer.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Systems/BuiltinDescriptorComponentSystems.hpp>
#include <BLIB/Render/Systems/BuiltinDrawableSystems.hpp>
#include <BLIB/Render/Systems/CameraUpdateSystem.hpp>
#include <BLIB/Render/Systems/OverlayScaler.hpp>
#include <BLIB/Render/Systems/RenderSystem.hpp>
#include <BLIB/Render/Systems/TextSyncSystem.hpp>
#include <cmath>

namespace bl
{
namespace gfx
{
Renderer::Renderer(engine::Engine& engine, sf::WindowBase& window)
: engine(engine)
, window(window)
, state(window)
, textures(state)
, materials(state)
, renderPasses(*this)
, descriptorSetFactoryCache(engine, *this)
, pipelineLayouts(*this)
, pipelines(*this)
, scenes(*this)
, splitscreenDirection(SplitscreenDirection::TopAndBottom)
, commonObserver(*this)
, defaultNear(0.1f)
, defaultFar(100.f) {
    renderTextures.reserve(16);
    commonObserver.setDefaultNearFar(defaultNear, defaultFar);
    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};
}

Renderer::~Renderer() {
    if (state.device != nullptr) { cleanup(); }
}

void Renderer::initialize() {
    renderRegion.width  = window.getSize().x;
    renderRegion.height = window.getSize().y;

    constexpr engine::StateMask::V StateMask = engine::StateMask::All;
    using engine::FrameStage;

    // core renderer systems
    engine.systems().registerSystem<sys::CameraUpdateSystem>(
        FrameStage::RenderObjectSync, StateMask, *this);
    engine.systems().registerSystem<sys::RenderSystem>(FrameStage::Render, StateMask, *this);
    engine.systems().registerSystem<sys::OverlayScaler>(FrameStage::RenderIntermediateRefresh,
                                                        StateMask);
    engine.systems().registerSystem<sys::TextSyncSystem>(FrameStage::RenderIntermediateRefresh,
                                                         StateMask);

    // descriptor systems
    engine.systems().registerSystem<sys::Transform2DDescriptorSystem>(
        FrameStage::RenderDescriptorRefresh, StateMask);
    engine.systems().registerSystem<sys::Transform3DDescriptorSystem>(
        FrameStage::RenderDescriptorRefresh, StateMask);
    engine.systems().registerSystem<sys::TextureDescriptorSystem>(
        FrameStage::RenderDescriptorRefresh, StateMask);

    // drawable systems
    engine.systems().registerSystem<sys::MeshSystem>(
        FrameStage::RenderObjectSync, StateMask, Config::PipelineIds::SkinnedMeshes);
    engine.systems().registerSystem<sys::SpriteSystem>(
        FrameStage::RenderObjectSync, StateMask, Config::PipelineIds::LitSkinned2DGeometry);

    // create renderer instance data
    state.init();
    renderPasses.addDefaults();
    textures.init();
    pipelines.createBuiltins();

    // swapchain framebuffers
    VkRenderPass renderPass =
        renderPasses.getRenderPass(Config::RenderPassIds::SwapchainPrimaryRender).rawPass();
    unsigned int i = 0;
    framebuffers.init(state.swapchain, [this, &i, renderPass](vk::Framebuffer& fb) {
        fb.create(state, renderPass, state.swapchain.swapFrameAtIndex(i));
        ++i;
    });

    // initialize observers
    addObserver();
    commonObserver.assignRegion(window.getSize(), renderRegion, 1, 0, true);
}

void Renderer::cleanup() {
    vkCheck(vkDeviceWaitIdle(state.device));

    for (vk::RenderTexture* rt : renderTextures) { rt->destroy(); }
    observers.clear();
    commonObserver.cleanup();
    scenes.cleanup();
    pipelines.cleanup();
    pipelineLayouts.cleanup();
    descriptorSetFactoryCache.cleanup();
    textures.cleanup();
    framebuffers.cleanup([](vk::Framebuffer& fb) { fb.cleanup(); });
    renderPasses.cleanup();
    state.cleanup();
    state.device = nullptr;
}

void Renderer::processResize(const sf::Rect<std::uint32_t>& region) {
    renderRegion = region;
    state.swapchain.invalidate();
    assignObserverRegions();
    commonObserver.assignRegion(window.getSize(), renderRegion, 1, 0, true);
}

void Renderer::updateCameras(float dt) {
    for (vk::RenderTexture* rt : renderTextures) { rt->updateCamera(dt); }
    commonObserver.updateCamera(dt);
    for (auto& o : observers) { o->updateCamera(dt); }
}

void Renderer::renderFrame() {
    // kick off transfers
    for (vk::RenderTexture* rt : renderTextures) { rt->handleDescriptorSync(); }
    if (commonObserver.hasScene()) { commonObserver.handleDescriptorSync(); }
    else {
        for (auto& o : observers) { o->handleDescriptorSync(); }
    }
    state.transferEngine.executeTransfers();

    // begin frame
    vk::StandardAttachmentSet* currentFrame = nullptr;
    VkCommandBuffer commandBuffer           = nullptr;
    state.beginFrame(currentFrame, commandBuffer);
    framebuffers.current().recreateIfChanged(*currentFrame);

    // begin render texture rendering
    for (vk::RenderTexture* rt : renderTextures) { rt->renderScene(commandBuffer); }

    // record commands to render scenes
    if (commonObserver.hasScene()) { commonObserver.renderScene(commandBuffer); }
    else {
        // record all before blocking to apply postfx
        for (auto& o : observers) { o->renderScene(commandBuffer); }
    }

    // begin render pass to composite content into swapchain image
    framebuffers.current().beginRender(commandBuffer,
                                       {{0, 0}, currentFrame->renderExtent()},
                                       clearColors,
                                       std::size(clearColors),
                                       false);

    // apply rendered scenes to swap image with postfx
    if (commonObserver.hasScene()) { commonObserver.compositeSceneWithEffects(commandBuffer); }
    else {
        for (auto& o : observers) { o->compositeSceneWithEffects(commandBuffer); }
    }

    // render overlays
    for (auto& o : observers) { o->renderOverlay(commandBuffer); }
    commonObserver.renderOverlay(commandBuffer);

    // complete frame
    framebuffers.current().finishRender(commandBuffer);
    state.completeFrame();
}

void Renderer::setDefaultNearAndFar(float n, float f) {
    defaultNear = n;
    defaultFar  = f;
    for (auto& o : observers) { o->setDefaultNearFar(n, f); }
}

Observer& Renderer::addObserver() {
#ifdef BLIB_DEBUG
    if (observers.size() == 4) {
        BL_LOG_CRITICAL << "Cannot add more than 4 observers";
        return *observers.back();
    }
#endif

    observers.emplace_back(new Observer(*this));
    assignObserverRegions();
    observers.back()->setDefaultNearFar(defaultNear, defaultFar);
    return *observers.back();
}

void Renderer::removeObserver(unsigned int i) {
    i = std::min(i, static_cast<unsigned int>(observers.size()) - 1);
    observers.erase(observers.begin() + i);
    assignObserverRegions();
}

void Renderer::popSceneFromAllObservers() {
    for (auto& o : observers) { o->popScene(); }
}

Scene* Renderer::popSceneFromAllObserversNoRelease() {
    Scene* s = nullptr;
    for (auto& o : observers) {
        Scene* ns = o->popSceneNoRelease();
#ifdef BLIB_DEBUG
        if (s != nullptr && ns != s) {
            BL_LOG_ERROR << "Popping scene without release but observers have different scenes";
        }
#endif
        s = ns;
    }
    return s;
}

unsigned int Renderer::observerCount() const { return observers.size(); }

void Renderer::assignObserverRegions() {
    unsigned int i = 0;
    for (auto& o : observers) {
        o->assignRegion(window.getSize(),
                        renderRegion,
                        observers.size(),
                        i,
                        splitscreenDirection == SplitscreenDirection::TopAndBottom);
        ++i;
    }
}

void Renderer::setSplitscreenDirection(SplitscreenDirection d) {
    splitscreenDirection = d;
    assignObserverRegions();
}

void Renderer::setClearColor(const glm::vec3& color) {
    clearColors[0].color = {color.x, color.y, color.z, 1.f};
}

void Renderer::registerRenderTexture(vk::RenderTexture* rt) { renderTextures.emplace_back(rt); }

void Renderer::removeRenderTexture(vk::RenderTexture* rt) {
    for (auto it = renderTextures.begin(); it != renderTextures.end(); ++it) {
        if (*it == rt) {
            renderTextures.erase(it);
            return;
        }
    }
}

} // namespace gfx
} // namespace bl

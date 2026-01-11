#include <BLIB/Render/Renderer.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/AutoExposureWorkBuffer.hpp>
#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>
#include <BLIB/Render/Graph/Assets/ShadowMapAsset.hpp>
#include <BLIB/Render/Graph/Assets/TaggedEmptyAsset.hpp>
#include <BLIB/Render/Graph/Providers/BloomProviders.hpp>
#include <BLIB/Render/Graph/Providers/GBufferProvider.hpp>
#include <BLIB/Render/Graph/Providers/GenericTargetProvider.hpp>
#include <BLIB/Render/Graph/Providers/SSAOProvider.hpp>
#include <BLIB/Render/Graph/Providers/SimpleAssetProvider.hpp>
#include <BLIB/Render/Graph/Providers/StandardTargetProvider.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>
#include <BLIB/Systems.hpp>
#include <cmath>

namespace bl
{
namespace rc
{
Renderer::Renderer(engine::Engine& engine, const CreationSettings& createSettings)
: engine(engine)
, windowScale(1.f)
, settings(*this, createSettings)
, state(window)
, swapchain(*this, window.getSfWindow(), settings.windowSettings)
, transferEngine(state)
, descriptorPool(state)
, globalDescriptors(engine, *this, textures, materials)
, textures(*this, state)
, materials(*this)
, samplers(*this)
, descriptorSetFactoryCache(engine, *this)
, renderPasses(*this)
, pipelineLayouts(*this)
, pipelines(*this)
, computePipelines(*this)
, materialPipelines(*this)
, scenes(engine)
, imageExporter(*this)
, splitscreenDirection(SplitscreenDirection::TopAndBottom)
, commonObserver(engine, *this, assetFactory, true, false)
, globalShaderResources(engine, commonObserver)
, sceneSync(engine.ecs()) {
    renderTextures.reserve(16);
    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};
    settings.emitter.connect(signalChannel);
    scenes.emitter.connect(signalChannel);
}

Renderer::~Renderer() {
    if (state.device != nullptr) { cleanup(); }
}

bool Renderer::initialize() {
    if (!createWindow()) { return false; }

    renderRegion.width  = window.getSfWindow().getSize().x;
    renderRegion.height = window.getSfWindow().getSize().y;

    constexpr engine::StateMask::V AllMask = engine::StateMask::All;
    using engine::FrameStage;

    textureFormatManager.init(*this);

    // core renderer systems
    engine.systems().registerSystem<sys::RendererUpdateSystem>(
        FrameStage::RendererDataSync, AllMask, *this);
    engine.systems().registerSystem<sys::OverlayScalerSystem>(FrameStage::RendererDataSync,
                                                              AllMask);
    engine.systems().registerSystem<sys::Animation2DSystem>(
        FrameStage::Animate,
        engine::StateMask::Running | engine::StateMask::Menu | engine::StateMask::Editor,
        *this);
    engine.systems().registerSystem<sys::SkeletalAnimationSystem>(
        FrameStage::Animate,
        engine::StateMask::Running | engine::StateMask::Menu | engine::StateMask::Editor);

    // create renderer instance data
    state.init();
    sharedCommandPool.create(state);
    swapchain.create();
    transferEngine.init();
    descriptorPool.init();
    shaderCache.init(state.getDevice());
    samplers.init();
    renderPasses.addDefaults();
    globalDescriptors.init();
    pipelines.createBuiltins();
    computePipelines.createBuiltins();
    materialPipelines.createBuiltins();

    // asset providers
    constexpr VkClearColorValue Zero              = {{0.f, 0.f, 0.f, 0.f}};
    constexpr VkClearColorValue Black             = {{0.f, 0.f, 0.f, 1.f}};
    constexpr VkClearColorValue Transparent       = {{0.f, 0.f, 0.f, 0.f}};
    constexpr VkClearDepthStencilValue ClearDepth = {1.f, 0};
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::DepthBuffer>>(rg::AssetTags::DepthBuffer,
                                                                         false);
    assetFactory.addProvider<rgi::StandardTargetProvider>(
        {rg::AssetTags::RenderedSceneOutput, rg::AssetTags::PostFXOutput},
        false,
        std::array<VkClearValue, 2>{VkClearValue{.color = Black},
                                    VkClearValue{.depthStencil = ClearDepth}});
    assetFactory.addProvider<rgi::BloomColorAttachmentPairProvider>(
        rg::AssetTags::BloomColorAttachmentPair,
        false,
        false,
        std::array<VkClearValue, 1>{VkClearValue{.color = Black}});
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::ShadowMapAsset>>(
        rg::AssetTags::ShadowMaps, false);
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::AutoExposureWorkBuffer>>(
        rg::AssetTags::AutoExposureWorkBuffer, false);
    constexpr std::array<VkClearValue, 5> GBufferClearValues{
        VkClearValue{.color = Transparent},
        VkClearValue{.color = Black},
        VkClearValue{.color = Zero},
        VkClearValue{.color = Zero},
        VkClearValue{.depthStencil = ClearDepth}};
    assetFactory.addProvider<rgi::GBufferProvider>(
        rg::AssetTags::GBuffer, false, GBufferClearValues);

    assetFactory.addProvider<rgi::SSAOProvider>(
        rg::AssetTags::SSAOBuffer,
        false,
        std::array<VkClearValue, 1>{VkClearValue{.color = {1.f, 1.f, 1.f, 1.f}}});
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::TaggedEmptyAsset>>(
        rg::AssetTags::ConsumedNextFrame, true);
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::TaggedEmptyAsset>>(
        rg::AssetTags::AutoExposureOutput, false);

    // initialize common observer
    commonObserver.init();
    commonObserver.assignRegion(window.getSfWindow().getSize(), renderRegion, 1, 0, true);
    assignObserverRegions();

    // begin ECS sync
    sceneSync.subscribe(signalChannel);

    // set up render strategies for scene types
    scene3DDeferredStrategy.init(*this);
    Overlay::useRenderStrategy(&overlayStrategy);
    scene::Scene2D::useRenderStrategy(&scene2DStrategy);
    scene::Scene3D::useRenderStrategy(&scene3DDeferredStrategy);

    return true;
}

bool Renderer::createWindow() {
    const auto& params = settings.windowSettings;
    settings.windowSettings.changesRequireNewWindow(); // resets internal state

    window.create(params.videoMode(), params.title(), params.style());
    if (!window.isOpen()) {
        BL_LOG_ERROR << "Failed to create window";
        return false;
    }

    if (!params.icon().empty()) {
        sf::Image icon;
        if (resource::ResourceManager<sf::Image>::initializeExisting(params.icon(), icon)) {
            window.getSfWindow().setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        }
        else { BL_LOG_WARN << "Failed to load icon: " << params.icon(); }
    }

    if (state.device) { processWindowRecreate(); }

    applySettingsToWindow();

    return true;
}

void Renderer::applySettingsToWindow() {
    const auto& params = settings.windowSettings;

    window.getSfWindow().setTitle(params.title());

    if (state.device) {
        swapchain.invalidate();

        sf::Event::SizeEvent e{};
        e.width  = window.getSfWindow().getSize().x;
        e.height = window.getSfWindow().getSize().y;
        processResize(e);
    }

    if (!params.letterBox() && params.syncOverlaySize()) {
        cam::OverlayCamera::setOverlayCoordinateSpace(window.getSfWindow().getSize().x,
                                                      window.getSfWindow().getSize().y);
    }
}

void Renderer::updateWindowFromSettings() {
    auto& params = settings.windowSettings;
    if (params.changesRequireNewWindow()) { createWindow(); }
    applySettingsToWindow();
}

void Renderer::earlyCleanup() { window.close(); }

void Renderer::cleanup() {
    vkCheck(vkDeviceWaitIdle(state.device));

    sceneSync.unsubscribe();
    imageExporter.cleanup();
    renderTextures.clear();
    observers.clear();
    virtualObservers.clear();
    commonObserver.cleanup();
    scenes.cleanup();
    globalShaderResources.cleanup();
    resource::ResourceManager<sf::VulkanFont>::freeAndDestroyAll();
    computePipelines.cleanup();
    pipelines.cleanup();
    pipelineLayouts.cleanup();
    descriptorSetFactoryCache.cleanup();
    globalDescriptors.cleanup();
    renderPasses.cleanup();
    samplers.cleanup();
    cleanupManager.flush();
    descriptorPool.cleanup();
    transferEngine.cleanup();
    shaderCache.cleanup();
    swapchain.destroy();
    sharedCommandPool.cleanup();
    state.cleanup();
    state.device = nullptr;
}

void Renderer::processResize(const sf::Event::SizeEvent& event) {
    auto windowSettings = settings.getWindowSettings();
    const sf::Vector2f modeSize(
        sf::Vector2u(windowSettings.videoMode().width, windowSettings.videoMode().height));
    const sf::Vector2f& ogSize =
        windowSettings.initialViewSize().x > 0.f ? windowSettings.initialViewSize() : modeSize;

    const float newWidth  = static_cast<float>(event.width);
    const float newHeight = static_cast<float>(event.height);

    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    if (windowSettings.letterBox()) {
        const float xScale = newWidth / ogSize.x;
        const float yScale = newHeight / ogSize.y;

        if (xScale >= yScale) { // constrained by height, bars on sides
            windowScale    = yScale;
            viewport.width = ogSize.x * yScale / newWidth;
            viewport.left  = (1.f - viewport.width) * 0.5f;
        }
        else { // constrained by width, bars on top and bottom
            windowScale     = xScale;
            viewport.height = ogSize.y * xScale / newHeight;
            viewport.top    = (1.f - viewport.height) * 0.5f;
        }
    }

    if (windowSettings.syncOverlaySize() && !windowSettings.letterBox()) {
        cam::OverlayCamera::setOverlayCoordinateSpace(newWidth, newHeight);
    }

    renderRegion = sf::Rect<std::uint32_t>(newWidth * viewport.left,
                                           newHeight * viewport.top,
                                           newWidth * viewport.width,
                                           newHeight * viewport.height);
    swapchain.invalidate();
    assignObserverRegions();
    commonObserver.assignRegion(window.getSfWindow().getSize(), renderRegion, 1, 0, true);
}

void Renderer::update(float dt, float realDt, float residual, float realResidual) {
    for (auto& rt : renderTextures) { rt->update(dt); }
    commonObserver.update(dt);
    for (auto& o : observers) { o->update(dt); }
    globalDescriptors.notifyUpdateTick(dt, realDt, residual, realResidual);
}

void Renderer::syncSceneObjects() {
    for (auto& rt : renderTextures) { rt->syncSceneObjects(); }
    commonObserver.syncSceneObjects();
    for (auto& o : observers) { o->syncSceneObjects(); }
}

void Renderer::copyDataFromSources() {
    globalShaderResources.updateFromSources();
    for (auto& rt : renderTextures) { rt->copyDataFromSources(); }
    if (commonObserver.hasScene()) { commonObserver.copyDataFromSources(); }
    else {
        for (auto& o : observers) { o->copyDataFromSources(); }
    }
}

void Renderer::renderFrame() {
    std::unique_lock lock(renderMutex);

    // begin frame
    vk::AttachmentSet* currentFrame = nullptr;
    VkCommandBuffer commandBuffer   = nullptr;
    swapchain.beginFrame(currentFrame, commandBuffer);
    cleanupManager.onFrameStart();

    // kick off transfers
    if (settings.dirty) {
        globalDescriptors.updateSettings(settings);
        settings.dirty = false;
    }
    globalDescriptors.onFrameStart();
    globalShaderResources.performTransfers();
    for (auto& rt : renderTextures) { rt->updateDescriptorsAndQueueTransfers(); }
    if (commonObserver.hasScene()) { commonObserver.updateDescriptorsAndQueueTransfers(); }
    else {
        for (auto& o : observers) { o->updateDescriptorsAndQueueTransfers(); }
    }
    transferEngine.executeTransfers();

    // reset graph asset states
    for (auto& rt : renderTextures) { rt->resetAssets(); }
    if (commonObserver.hasScene()) { commonObserver.resetAssets(); }
    else {
        for (auto& o : observers) { o->resetAssets(); }
    }

    // render offscreen textures
    for (auto& rt : renderTextures) { rt->render(); }

    // record commands to render scenes
    for (auto& o : observers) { o->renderScene(commandBuffer); }
    if (!virtualObservers.empty()) {
        for (auto& o : virtualObservers) { o->renderScene(commandBuffer); }
    }
    if (commonObserver.hasScene()) { commonObserver.renderScene(commandBuffer); }

    // complete frame
    swapchain.completeFrame();
    state.incrementFrame();

    // submit texture exports
    imageExporter.onFrameEnd();
}

Observer& Renderer::addObserver() {
    std::unique_lock lock(renderMutex);

#ifdef BLIB_DEBUG
    if (observers.size() == 4) {
        BL_LOG_CRITICAL << "Cannot add more than 4 observers";
        return *observers.back();
    }
#endif

    auto& observer =
        *observers.emplace_back(new Observer(engine, *this, assetFactory, false, false));
    if (state.device) {
        observer.init();
        assignObserverRegions();
    }
    return observer;
}

void Renderer::removeObserver(unsigned int i) {
    std::unique_lock lock(renderMutex);

    i = std::min(i, static_cast<unsigned int>(observers.size()) - 1);
    observers.erase(observers.begin() + i);
    assignObserverRegions();
}

void Renderer::popSceneFromAllObservers() {
    for (auto& o : observers) { o->popScene(); }
}

unsigned int Renderer::observerCount() const { return observers.size(); }

Observer& Renderer::addVirtualObserver(const VkRect2D& region) {
    std::unique_lock lock(renderMutex);

    auto& vobs =
        *virtualObservers.emplace_back(new Observer(engine, *this, assetFactory, false, true));
    vobs.init();
    vobs.assignRegion(region);
    return vobs;
}

void Renderer::destroyVirtualObserver(const Observer& o) {
    vkCheck(vkDeviceWaitIdle(state.device));
    for (auto it = virtualObservers.begin(); it != virtualObservers.end(); ++it) {
        if (it->get() == &o) {
            virtualObservers.erase(it);
            return;
        }
    }
    BL_LOG_WARN << "Failed to find virtual observer " << &o;
}

void Renderer::assignObserverRegions() {
    unsigned int i = 0;
    for (auto& o : observers) {
        o->assignRegion(window.getSfWindow().getSize(),
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

void Renderer::setClearColor(const Color& color) {
    clearColors[0].color = {{color.r(), color.g(), color.b(), 1.f}};
}

vk::RenderTexture::Handle Renderer::createRenderTexture(const glm::u32vec2& size,
                                                        vk::SamplerOptions::Type sampler) {
    std::unique_lock lock(renderMutex);

    renderTextures.emplace_back(new vk::RenderTexture(engine, *this, assetFactory, size, sampler));
    renderTextures.back()->init();
    return vk::RenderTexture::Handle(this, renderTextures.back().get());
}

void Renderer::destroyRenderTexture(vk::RenderTexture* rt) {
    std::unique_lock lock(renderMutex);

    rt->destroy();

    cleanupManager.add([this, rt]() {
        for (auto it = renderTextures.begin(); it != renderTextures.end(); ++it) {
            if (it->get() == rt) {
                renderTextures.erase(it);
                return;
            }
        }
    });
}

void Renderer::processWindowRecreate() {
    state.createSurface(cleanupManager);
    swapchain.invalidate();
}

CreationSettings Renderer::getCreationSettings() const {
    CreationSettings s;
    s.withGraphicsSettings(settings.graphicsSettings);
    s.withWindowSettings(settings.windowSettings);
    return s;
}

} // namespace rc
} // namespace bl

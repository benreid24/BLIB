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
Renderer::Renderer(engine::Engine& engine, engine::EngineWindow& window)
: engine(engine)
, window(window)
, settings(*this)
, state(window)
, globalDescriptors(engine, *this, textures, materials)
, textures(*this, state)
, materials(*this)
, samplers(state)
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
, globalShaderResources(engine)
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

void Renderer::initialize() {
    renderRegion.width  = window.getSfWindow().getSize().x;
    renderRegion.height = window.getSfWindow().getSize().y;

    constexpr engine::StateMask::V AllMask = engine::StateMask::All;
    using engine::FrameStage;

    state.textureFormatManager.init(*this);

    // core renderer systems
    engine.systems().registerSystem<sys::RendererUpdateSystem>(
        FrameStage::RenderEarlyRefresh, AllMask, *this);
    engine.systems().registerSystem<sys::OverlayScalerSystem>(FrameStage::RenderEarlyRefresh,
                                                              AllMask);
    engine.systems().registerSystem<sys::Animation2DSystem>(
        FrameStage::Animate,
        engine::StateMask::Running | engine::StateMask::Menu | engine::StateMask::Editor,
        *this);

    // create renderer instance data
    state.init();
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
        rgi::TargetSize(rgi::TargetSize::ObserverSize),
        std::array<vk::SemanticTextureFormat, 1>{vk::SemanticTextureFormat::Color},
        std::array<VkImageUsageFlags, 1>{VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                         VK_IMAGE_USAGE_SAMPLED_BIT},
        std::array<VkClearValue, 2>{VkClearValue{.color = Black},
                                    VkClearValue{.depthStencil = ClearDepth}});
    assetFactory.addProvider<rgi::BloomColorAttachmentPairProvider>(
        rg::AssetTags::BloomColorAttachmentPair,
        false,
        false,
        rgi::TargetSize(rgi::TargetSize::ObserverSize),
        std::array<vk::SemanticTextureFormat, 1>{vk::SemanticTextureFormat::SFloatR16G16B16A16},
        std::array<VkImageUsageFlags, 1>{VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                         VK_IMAGE_USAGE_SAMPLED_BIT},
        std::array<VkClearValue, 1>{VkClearValue{.color = Black}});
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::ShadowMapAsset>>(
        rg::AssetTags::ShadowMaps, false);
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::AutoExposureWorkBuffer>>(
        rg::AssetTags::AutoExposureWorkBuffer, false);
    constexpr std::array<VkImageUsageFlags, 4> GBufferUsages{
        // albedo
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        // specular + shininess
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        // positions + lighting on/off
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        // normals
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT};
    constexpr std::array<VkClearValue, 5> GBufferClearValues{
        VkClearValue{.color = Transparent},
        VkClearValue{.color = Black},
        VkClearValue{.color = Zero},
        VkClearValue{.color = Zero},
        VkClearValue{.depthStencil = ClearDepth}};
    assetFactory.addProvider<rgi::GBufferProvider>(
        rg::AssetTags::GBuffer,
        false,
        rgi::TargetSize(rgi::TargetSize::ObserverSize),
        std::array<vk::SemanticTextureFormat, 4>{vk::SemanticTextureFormat::Color,
                                                 vk::SemanticTextureFormat::Color,
                                                 vk::SemanticTextureFormat::HighPrecisionColor,
                                                 vk::SemanticTextureFormat::HighPrecisionColor},
        GBufferUsages,
        GBufferClearValues);

    assetFactory.addProvider<rgi::SSAOProvider>(
        rg::AssetTags::SSAOBuffer,
        false,
        rgi::TargetSize(rgi::TargetSize::ObserverSize),
        std::array<vk::SemanticTextureFormat, 1>{vk::SemanticTextureFormat::SingleChannelUnorm8},
        std::array<VkImageUsageFlags, 1>{VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                         VK_IMAGE_USAGE_SAMPLED_BIT},
        std::array<VkClearValue, 1>{VkClearValue{.color = {1.f, 1.f, 1.f, 1.f}}});
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::TaggedEmptyAsset>>(
        rg::AssetTags::ConsumedNextFrame, true);
    assetFactory.addProvider<rgi::SimpleAssetProvider<rgi::TaggedEmptyAsset>>(
        rg::AssetTags::AutoExposureOutput, false);

    // initialize common observer
    commonObserver.assignRegion(window.getSfWindow().getSize(), renderRegion, 1, 0, true);
    assignObserverRegions();

    // begin ECS sync
    sceneSync.subscribe(signalChannel);

    // set up render strategies for scene types
    scene3DDeferredStrategy.init(*this);
    Overlay::useRenderStrategy(&overlayStrategy);
    scene::Scene2D::useRenderStrategy(&scene2DStrategy);
    scene::Scene3D::useRenderStrategy(&scene3DDeferredStrategy);
}

void Renderer::cleanup() {
    vkCheck(vkDeviceWaitIdle(state.device));

    sceneSync.unsubscribe();
    imageExporter.cleanup();
    resource::ResourceManager<sf::VulkanFont>::freeAndDestroyAll();
    renderTextures.clear();
    observers.clear();
    virtualObservers.clear();
    commonObserver.cleanup();
    scenes.cleanup();
    computePipelines.cleanup();
    pipelines.cleanup();
    pipelineLayouts.cleanup();
    descriptorSetFactoryCache.cleanup();
    globalDescriptors.cleanup();
    renderPasses.cleanup();
    samplers.cleanup();
    state.cleanup();
    state.device = nullptr;
}

void Renderer::processResize(const sf::Rect<std::uint32_t>& region) {
    renderRegion = region;
    state.swapchain.invalidate();
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

void Renderer::renderFrame() {
    std::unique_lock lock(renderMutex);

    // begin frame
    vk::AttachmentSet* currentFrame = nullptr;
    VkCommandBuffer commandBuffer   = nullptr;
    state.beginFrame(currentFrame, commandBuffer);

    // kick off transfers
    if (settings.dirty) {
        globalDescriptors.updateSettings(settings);
        settings.dirty = false;
    }
    globalDescriptors.onFrameStart();
    for (auto& rt : renderTextures) { rt->updateDescriptorsAndQueueTransfers(); }
    if (commonObserver.hasScene()) { commonObserver.updateDescriptorsAndQueueTransfers(); }
    else {
        for (auto& o : observers) { o->updateDescriptorsAndQueueTransfers(); }
    }
    state.transferEngine.executeTransfers();

    // reset graph asset states
    for (auto& rt : renderTextures) { rt->resetAssets(); }
    if (commonObserver.hasScene()) { commonObserver.resetAssets(); }
    else {
        for (auto& o : observers) { o->resetAssets(); }
    }

    // render offscreen textures
    for (auto& rt : renderTextures) { rt->render(); }

    const auto clearDepthBuffer = [this, commandBuffer]() {
        VkClearAttachment attachment{};
        attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        attachment.clearValue = clearColors[1];

        VkClearRect rect{};
        rect.rect.extent.width  = renderRegion.width;
        rect.rect.extent.height = renderRegion.height;
        rect.rect.offset.x      = renderRegion.left;
        rect.rect.offset.y      = renderRegion.top;
        rect.baseArrayLayer     = 0;
        rect.layerCount         = 1;

        vkCmdClearAttachments(commandBuffer, 1, &attachment, 1, &rect);
    };

    // record commands to render scenes
    for (auto& o : observers) { o->renderScene(commandBuffer); }
    if (!virtualObservers.empty()) {
        for (auto& o : virtualObservers) { o->renderScene(commandBuffer); }
    }
    if (commonObserver.hasScene()) { commonObserver.renderScene(commandBuffer); }

    // complete frame
    state.completeFrame();

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

    observers.emplace_back(new Observer(engine, *this, assetFactory, false, false));
    if (state.device) { assignObserverRegions(); }
    return *observers.back();
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

    virtualObservers.emplace_back(new Observer(engine, *this, assetFactory, false, true));
    virtualObservers.back()->assignRegion(region);
    return *observers.back();
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
    return vk::RenderTexture::Handle(this, renderTextures.back().get());
}

void Renderer::destroyRenderTexture(vk::RenderTexture* rt) {
    std::unique_lock lock(renderMutex);

    rt->destroy();

    vulkanState().cleanupManager.add([this, rt]() {
        for (auto it = renderTextures.begin(); it != renderTextures.end(); ++it) {
            if (it->get() == rt) {
                renderTextures.erase(it);
                return;
            }
        }
    });
}

void Renderer::processWindowRecreate() {
    state.createSurface();
    state.swapchain.invalidate();
}

} // namespace rc
} // namespace bl

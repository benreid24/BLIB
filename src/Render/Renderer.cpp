#include <BLIB/Render/Renderer.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Providers/StandardTargetProvider.hpp>
#include <BLIB/Render/Graph/Strategies/ForwardRenderStrategy.hpp>
#include <BLIB/Systems.hpp>
#include <BLIB/Systems/SceneObjectRemovalSystem.hpp>
#include <cmath>

namespace bl
{
namespace rc
{
Renderer::Renderer(engine::Engine& engine, engine::EngineWindow& window)
: engine(engine)
, window(window)
, state(window)
, textures(state)
, materials(state)
, descriptorSetFactoryCache(engine, *this)
, renderPasses(*this)
, pipelineLayouts(*this)
, pipelines(*this)
, scenes(engine)
, splitscreenDirection(SplitscreenDirection::TopAndBottom)
, commonObserver(engine, *this, assetFactory, true) {
    renderTextures.reserve(16);
    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};
}

Renderer::~Renderer() {
    if (state.device != nullptr) { cleanup(); }
}

void Renderer::initialize() {
    renderRegion.width  = window.getSfWindow().getSize().x;
    renderRegion.height = window.getSfWindow().getSize().y;

    constexpr engine::StateMask::V StateMask = engine::StateMask::All;
    using engine::FrameStage;

    // core renderer systems
    engine.systems().registerSystem<sys::RendererUpdateSystem>(
        FrameStage::RenderObjectInsertion, StateMask, *this);
    engine.systems().registerSystem<sys::RenderSystem>(FrameStage::Render, StateMask, *this);
    engine.systems().registerSystem<sys::OverlayScalerSystem>(FrameStage::RenderIntermediateRefresh,
                                                              StateMask);
    engine.systems().registerSystem<sys::Animation2DSystem>(
        FrameStage::Animate, engine::StateMask::Running | engine::StateMask::Menu, *this);
    engine.systems().registerSystem<sys::SceneObjectRemovalSystem>(FrameStage::RenderObjectRemoval,
                                                                   StateMask);

    // descriptor systems
    engine.systems().registerSystem<sys::Transform2DDescriptorSystem>(
        FrameStage::RenderDescriptorRefresh, StateMask);
    engine.systems().registerSystem<sys::Transform3DDescriptorSystem>(
        FrameStage::RenderDescriptorRefresh, StateMask);
    engine.systems().registerSystem<sys::TextureDescriptorSystem>(
        FrameStage::RenderDescriptorRefresh, StateMask);

    // drawable systems
    engine.systems().registerSystem<sys::MeshSystem>(FrameStage::RenderObjectInsertion,
                                                     StateMask,
                                                     Config::PipelineIds::SkinnedMeshes,
                                                     Config::PipelineIds::SkinnedMeshes);
    engine.systems().registerSystem<sys::SpriteSystem>(FrameStage::RenderObjectInsertion,
                                                       StateMask,
                                                       Config::PipelineIds::LitSkinned2DGeometry,
                                                       Config::PipelineIds::UnlitSkinned2DGeometry);
    engine.systems().registerSystem<sys::BatchedSpriteSystem>(
        FrameStage::RenderObjectInsertion,
        StateMask,
        Config::PipelineIds::LitSkinned2DGeometry,
        Config::PipelineIds::UnlitSkinned2DGeometry);
    engine.systems().registerSystem<sys::TextSystem>(FrameStage::RenderObjectInsertion,
                                                     StateMask,
                                                     Config::PipelineIds::Text,
                                                     Config::PipelineIds::Text);
    engine.systems().registerSystem<sys::SlideshowSystem>(FrameStage::RenderObjectInsertion,
                                                          StateMask,
                                                          Config::PipelineIds::SlideshowLit,
                                                          Config::PipelineIds::SlideshowUnlit);
    engine.systems().registerSystem<sys::Animation2DDrawableSystem>(
        FrameStage::RenderObjectInsertion,
        StateMask,
        Config::PipelineIds::LitSkinned2DGeometry,
        Config::PipelineIds::UnlitSkinned2DGeometry);
    engine.systems().registerSystem<sys::Shape2DSystem>(FrameStage::RenderObjectInsertion,
                                                        StateMask,
                                                        Config::PipelineIds::Lit2DGeometry,
                                                        Config::PipelineIds::Unlit2DGeometry);
    engine.systems().registerSystem<sys::BatchedShapes2DSystem>(
        FrameStage::RenderObjectInsertion,
        StateMask,
        Config::PipelineIds::Lit2DGeometry,
        Config::PipelineIds::Unlit2DGeometry);

    // asset providers
    assetFactory.addProvider<rgi::StandardAssetProvider>(rg::AssetTags::RenderedSceneOutput);
    assetFactory.addProvider<rgi::StandardAssetProvider>(rg::AssetTags::PostFXOutput);

    // create renderer instance data
    state.init();
    renderPasses.addDefaults();
    textures.init();
    pipelines.createBuiltins();

    // swapchain framebuffers
    VkRenderPass renderPass =
        renderPasses.getRenderPass(Config::RenderPassIds::SwapchainDefault).rawPass();
    unsigned int i = 0;
    framebuffers.init(state.swapchain, [this, &i, renderPass](vk::Framebuffer& fb) {
        fb.create(state, renderPass, state.swapchain.swapFrameAtIndex(i));
        ++i;
    });

    // initialize observers
    addObserver();
    commonObserver.assignRegion(window.getSfWindow().getSize(), renderRegion, 1, 0, true);
}

void Renderer::cleanup() {
    vkCheck(vkDeviceWaitIdle(state.device));

    resource::ResourceManager<sf::VulkanFont>::freeAndDestroyAll();
    engine.systems().getSystem<sys::Animation2DSystem>().cleanup();
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
    commonObserver.assignRegion(window.getSfWindow().getSize(), renderRegion, 1, 0, true);
}

void Renderer::update(float dt) {
    for (vk::RenderTexture* rt : renderTextures) { rt->updateCamera(dt); }
    commonObserver.update(dt);
    for (auto& o : observers) { o->update(dt); }
}

void Renderer::renderFrame() {
    // kick off transfers
    textures.onFrameStart();
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
    if (commonObserver.hasScene()) {
        clearDepthBuffer();
        commonObserver.renderScene(commandBuffer);
    }

    // perform render pass for final scene renders and overlays
    framebuffers.current().beginRender(commandBuffer,
                                       {{0, 0}, currentFrame->renderExtent()},
                                       clearColors,
                                       std::size(clearColors),
                                       false);

    // render scene outputs
    for (auto& o : observers) { o->compositeSceneAndOverlay(commandBuffer); }
    if (commonObserver.hasScene()) {
        clearDepthBuffer();
        commonObserver.compositeSceneAndOverlay(commandBuffer);
    }

    // complete frame
    framebuffers.current().finishRender(commandBuffer);
    state.completeFrame();
}

Observer& Renderer::addObserver() {
#ifdef BLIB_DEBUG
    if (observers.size() == 4) {
        BL_LOG_CRITICAL << "Cannot add more than 4 observers";
        return *observers.back();
    }
#endif

    observers.emplace_back(new Observer(engine, *this, assetFactory, false));
    assignObserverRegions();
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

unsigned int Renderer::observerCount() const { return observers.size(); }

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

void Renderer::setClearColor(const glm::vec3& color) {
    clearColors[0].color = {{color.x, color.y, color.z, 1.f}};
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

rg::Strategy& Renderer::getRenderStrategy() {
    if (!strategy) { useRenderStrategy<rgi::ForwardRenderStrategy>(); }
    return *strategy;
}

} // namespace rc
} // namespace bl

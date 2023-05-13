#include <BLIB/Render/Renderer.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Scenes/StagePipelines.hpp>
#include <BLIB/Render/Systems/BuiltinDescriptorComponentSystems.hpp>
#include <BLIB/Render/Systems/BuiltinDrawableSystems.hpp>
#include <cmath>

namespace bl
{
namespace render
{
Renderer::Renderer(engine::Engine& engine, sf::WindowBase& window)
: engine(engine)
, window(window)
, state(window)
, textures(state)
, materials(state)
, renderPasses(*this)
, descriptorSetFactoryCache(engine, *this)
, pipelines(*this)
, scenes(*this)
, splitscreenDirection(SplitscreenDirection::TopAndBottom)
, commonObserver(*this)
, defaultNear(0.1f)
, defaultFar(100.f) {
    commonObserver.setDefaultNearFar(defaultNear, defaultFar);
    clearColor = {{{0.f, 0.f, 0.f, 1.f}}};
}

Renderer::~Renderer() {
    if (state.device != nullptr) { cleanup(); }
}

void Renderer::initialize() {
    renderRegion.width  = window.getSize().x;
    renderRegion.height = window.getSize().y;

    // register systems
    engine.systems().registerSystem<sys::Transform3DDescriptorSystem>(
        engine::FrameStage::RenderDescriptorRefresh, engine::StateMask::All);
    engine.systems().registerSystem<sys::TextureDescriptorSystem>(
        engine::FrameStage::RenderDescriptorRefresh, engine::StateMask::All);
    engine.systems().registerSystem<sys::MeshSystem>(
        engine::FrameStage::RenderObjectSync,
        engine::StateMask::All,
        scene::StagePipelineBuilder()
            .withPipeline(Config::SceneObjectStage::OpaquePass,
                          Config::PipelineIds::OpaqueSkinnedMeshes)
            .withPipeline(Config::SceneObjectStage::TransparentPass, // TODO - opaque mesh pipeline
                          Config::PipelineIds::None)
            .build());

    // create renderer instance data
    state.init();
    renderPasses.addDefaults();
    textures.init();
    pipelines.createBuiltins();

    // swapchain framebuffers
    VkRenderPass renderPass =
        renderPasses.getRenderPass(Config::RenderPassIds::SwapchainPrimaryRender).rawPass();
    unsigned int i = 0;
    framebuffers.init(state.swapchain, [this, &i, renderPass](Framebuffer& fb) {
        fb.create(state, renderPass, state.swapchain.swapFrameAtIndex(i));
        ++i;
    });

    // initialize observers
    addObserver();
    commonObserver.assignRegion(window.getSize(), renderRegion, 1, 0, true);
}

void Renderer::cleanup() {
    vkCheck(vkDeviceWaitIdle(state.device));

    observers.clear();
    commonObserver.cleanup();
    scenes.cleanup();
    pipelines.cleanup();
    descriptorSetFactoryCache.cleanup();
    textures.cleanup();
    framebuffers.cleanup([](Framebuffer& fb) { fb.cleanup(); });
    renderPasses.cleanup();
    state.cleanup();
    state.device = nullptr;
}

void Renderer::processResize(const sf::Rect<std::uint32_t>& region) {
    std::unique_lock lock(mutex);

    renderRegion = region;
    state.swapchain.invalidate();
    assignObserverRegions();
    commonObserver.assignRegion(window.getSize(), renderRegion, 1, 0, true);
}

void Renderer::update(float dt) {
    std::unique_lock lock(mutex);
    commonObserver.update(dt);
    for (auto& o : observers) { o->update(dt); }
}

void Renderer::renderFrame() {
    std::unique_lock lock(mutex);

    // execute transfers
    if (commonObserver.hasScene()) { commonObserver.handleDescriptorSync(); }
    else {
        for (auto& o : observers) { o->handleDescriptorSync(); }
    }
    scenes.handleDescriptorSync();
    state.transferEngine.executeTransfers();

    // begin frame
    ColorAttachmentSet* currentFrame = nullptr;
    VkCommandBuffer commandBuffer    = nullptr;
    state.beginFrame(currentFrame, commandBuffer);
    framebuffers.current().recreateIfChanged(*currentFrame);

    // record commands to render scenes
    if (commonObserver.hasScene()) { commonObserver.renderScene(commandBuffer); }
    else {
        // record all before blocking to apply postfx
        for (auto& o : observers) { o->renderScene(commandBuffer); }
    }

    // wait for scenes to complete rendering
    if (commonObserver.hasScene()) { commonObserver.insertSceneBarriers(commandBuffer); }
    else {
        // record all before blocking to apply postfx
        for (auto& o : observers) { o->insertSceneBarriers(commandBuffer); }
    }

    // begin render pass to composite content into swapchain image
    framebuffers.current().beginRender(
        commandBuffer, {{0, 0}, currentFrame->renderExtent()}, &clearColor, 1, true);

    // apply rendered scenes to swap image with postfx
    if (commonObserver.hasScene()) { commonObserver.compositeSceneWithEffects(commandBuffer); }
    else {
        for (auto& o : observers) { o->compositeSceneWithEffects(commandBuffer); }
    }

    // TODO - render overlays

    framebuffers.current().finishRender(commandBuffer);

    state.completeFrame();
}

void Renderer::setDefaultNearAndFar(float n, float f) {
    std::unique_lock lock(mutex);

    defaultNear = n;
    defaultFar  = f;
    for (auto& o : observers) { o->setDefaultNearFar(n, f); }
}

Observer& Renderer::addObserver() {
    std::unique_lock lock(mutex);

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
    std::unique_lock lock(mutex);
    i = std::min(i, static_cast<unsigned int>(observers.size()) - 1);
    observers.erase(observers.begin() + i);
    assignObserverRegions();
}

Scene* Renderer::pushSceneToAllObservers(std::uint32_t maxStaticObjectCount,
                                         std::uint32_t maxDynamicObjectCount) {
    std::unique_lock lock(mutex);

    Scene* s = scenes.allocateScene(maxStaticObjectCount, maxDynamicObjectCount);
    for (auto& o : observers) { o->pushScene(s); }
    return s;
}

void Renderer::popSceneFromAllObservers(bool cams) {
    std::unique_lock lock(mutex);
    for (auto& o : observers) { o->popScene(cams); }
}

Scene* Renderer::popSceneFromAllObserversNoRelease(bool cams) {
    std::unique_lock lock(mutex);
    Scene* s = nullptr;
    for (auto& o : observers) {
        Scene* ns = o->popSceneNoRelease(cams);
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

void Renderer::setSplitscreenDirection(SplitscreenDirection d) { splitscreenDirection = d; }

void Renderer::setClearColor(const VkClearColorValue& color) { clearColor.color = color; }

} // namespace render
} // namespace bl

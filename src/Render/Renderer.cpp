#include <BLIB/Render/Renderer.hpp>

#include <cmath>

namespace bl
{
namespace render
{
Renderer::Renderer(sf::WindowBase& window)
: window(window)
, state(window)
, textures(state)
, materials(state)
, renderPasses(*this)
, pipelines(*this)
, scenes(*this)
, splitscreenDirection(SplitscreenDirection::TopAndBottom)
, commonObserver(*this)
, defaultNear(0.1f)
, defaultFar(100.f) {
    commonObserver.setDefaultNearFar(defaultNear, defaultFar);
}

Renderer::~Renderer() {
    if (state.device != nullptr) { cleanup(); }
}

void Renderer::initialize() {
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

    addObserver();
    commonObserver.assignRegion(window, 1, 0, true);
}

void Renderer::cleanup() {
    vkDeviceWaitIdle(state.device);

    observers.clear();
    commonObserver.cleanup();
    scenes.cleanup();
    pipelines.cleanup();
    textures.cleanup();
    framebuffers.cleanup([](Framebuffer& fb) { fb.cleanup(); });
    renderPasses.cleanup();
    state.cleanup();
    state.device = nullptr;
}

void Renderer::processResize() {
    std::unique_lock lock(mutex);

    state.swapchain.invalidate();
    assignObserverRegions();
    commonObserver.assignRegion(window, 1, 0, true);
}

void Renderer::update(float dt) {
    std::unique_lock lock(mutex);
    commonObserver.update(dt);
    for (auto& o : observers) { o->update(dt); }
}

void Renderer::renderFrame() {
    std::unique_lock lock(mutex);

    // execute transfers
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
    VkClearValue clearColors[1];
    clearColors[0] = {{{0.f, 0.f, 0.f, 1.f}}};
    framebuffers.current().beginRender(commandBuffer,
                                       {{0, 0}, currentFrame->renderExtent()},
                                       clearColors,
                                       std::size(clearColors),
                                       true);

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

Scene* Renderer::pushSceneToAllObservers() {
    std::unique_lock lock(mutex);

    Scene* s = scenes.allocateScene();
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
        o->assignRegion(window,
                        observers.size(),
                        i,
                        splitscreenDirection == SplitscreenDirection::TopAndBottom);
        ++i;
    }
}

void Renderer::setSplitscreenDirection(SplitscreenDirection d) { splitscreenDirection = d; }

} // namespace render
} // namespace bl

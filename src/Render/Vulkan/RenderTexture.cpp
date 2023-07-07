#include <BLIB/Render/Vulkan/RenderTexture.hpp>

#include <BLIB/Render/Cameras/2D/Camera2D.hpp>
#include <BLIB/Render/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
RenderTexture::RenderTexture()
: renderer(nullptr)
, scene(nullptr)
, defaultNear(0.1f)
, defaultFar(100.f) {
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};
}

RenderTexture::~RenderTexture() { destroy(); }

void RenderTexture::create(Renderer& r, const glm::u32vec2& size, VkSampler sampler) {
    const bool firstInit = renderer == nullptr;

    renderer = &r;
    r.registerRenderTexture(this);
    defaultNear = r.defaultNearPlane();
    defaultFar  = r.defaultFarPlane();

    // viewport and scissor come from size
    scissor.extent.width  = size.x;
    scissor.extent.height = size.y;
    scissor.offset        = {0, 0};
    viewport              = ovy::Viewport::scissorToViewport(scissor);

    // ensure we have a camera
    ensureCamera();

    // allocate textures and depth buffers
    if (firstInit) { texture = renderer->texturePool().createRenderTexture(size, sampler); }
    else { texture->ensureSize(size); }
    depthBuffer.create(renderer->vulkanState(),
                       StandardAttachmentBuffers::findDepthFormat(renderer->vulkanState()),
                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                       {size.x, size.y});

    // set attachment sets and bind framebuffers
    VkRenderPass renderPass =
        r.renderPassCache().getRenderPass(Config::RenderPassIds::OffScreenSceneRender).rawPass();
    attachmentSet.setRenderExtent(scissor.extent);
    attachmentSet.setAttachments(
        texture->getImage(), texture->getView(), depthBuffer.image(), depthBuffer.view());
    framebuffer.create(r.vulkanState(), renderPass, attachmentSet);
}

void RenderTexture::destroy() {
    if (renderer) {
        renderer->removeRenderTexture(this);
        framebuffer.cleanup();
        depthBuffer.destroy();
        renderer->texturePool().releaseTexture(texture);
        renderer = nullptr;
        // TODO - should we refcount scenes?
    }
}

void RenderTexture::setScene(Scene* s) {
    scene = s;
    onSceneSet();
}

void RenderTexture::onSceneSet() {
    observerIndex = scene->registerObserver();
    ensureCamera();
}

void RenderTexture::removeScene(bool cam) {
    scene = nullptr;
    if (cam) { camera.release(); }
}

void RenderTexture::setClearColor(const glm::vec4& color) {
    clearColors[0].color = {color.x, color.y, color.z, color.w};
}

void RenderTexture::ensureCamera() {
    if (!camera) {
#if SCENE_DEFAULT_CAMERA == 2
        setCamera<c2d::Camera2D>(
            glm::vec2{viewport.x + viewport.width * 0.5f, viewport.y + viewport.height * 0.5f},
            glm::vec2{viewport.width, viewport.height});
#else
        setCamera<c3d::Camera3D>();
#endif
    }
}

void RenderTexture::handleDescriptorSync() {
    if (hasScene()) {
        const glm::mat4 projView = camera->getProjectionMatrix(viewport) * camera->getViewMatrix();
        scene->updateObserverCamera(observerIndex, projView);
        scene->handleDescriptorSync();
    }
}

void RenderTexture::updateCamera(float dt) {
    if (camera) { camera->update(dt); }
}

void RenderTexture::renderScene(VkCommandBuffer commandBuffer) {
    const VkRect2D renderRegion{{0, 0}, attachmentSet.renderExtent()};
    framebuffer.beginRender(commandBuffer, renderRegion, clearColors, std::size(clearColors), true);

    if (hasScene()) {
        scene::SceneRenderContext ctx(commandBuffer,
                                      observerIndex,
                                      viewport,
                                      Config::RenderPassIds::OffScreenSceneRender,
                                      true);
        scene->renderScene(ctx);
    }

    framebuffer.finishRender(commandBuffer);
}

} // namespace vk
} // namespace gfx
} // namespace bl
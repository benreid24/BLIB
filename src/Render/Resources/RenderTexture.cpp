#include <BLIB/Render/Resources/RenderTexture.hpp>

#include <BLIB/Render/Overlays/Viewport.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace gfx
{
namespace res
{
RenderTexture::RenderTexture()
: renderer(nullptr)
, scene(nullptr) {
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    clearColors[0].color        = {{0.f, 0.f, 0.f, 1.f}};
    clearColors[1].depthStencil = {1.f, 0};
}

RenderTexture::~RenderTexture() {
    if (renderer != nullptr) { destroy(); }
}

void RenderTexture::create(Renderer& r, const glm::u32vec2& size, VkSampler sampler) {
    if (renderer != nullptr) { destroy(); }
    renderer = &r;

    // viewport and scissor come from size
    scissor.extent.width  = size.x;
    scissor.extent.height = size.y;
    scissor.offset        = {0, 0};
    viewport              = ovy::Viewport::scissorToViewport(scissor);

    // ensure we have a camera
    ensureCamera();

    // allocate textures and depth buffers
    textures.init(r.vulkanState(), [this, &size](TextureRef& txtr) {
        txtr = renderer->texturePool().createTexture(size, sampler);
    });
    depthBuffers.init(r.vulkanState(), [this](vk::AttachmentBuffer& db) {
        db.create(renderer->vulkanState(),
                  StandardAttachmentBuffers::findDepthFormat(renderer->vulkanState()),
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                  {size.x, size.y});
    });

    // TODO - init rest
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

} // namespace res
} // namespace gfx
} // namespace bl

#include <BLIB/Render/Vulkan/RenderTexture.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Graph/Assets/FinalRenderTextureAsset.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
RenderTexture::RenderTexture(engine::Engine& engine, Renderer& renderer, rg::AssetFactory& factory,
                             const glm::u32vec2& size, VkSampler sampler)
: RenderTarget(engine, renderer, factory, true) {
    commandPool.create(renderer.vulkanState());

    scissor.extent.width  = size.x;
    scissor.extent.height = size.y;
    scissor.offset        = {0, 0};
    viewport.minDepth     = 0.f;
    viewport.maxDepth     = 1.f;
    viewport.x            = 0.f;
    viewport.y            = 0.f;
    viewport.width        = size.x;
    viewport.height       = size.y;

    // allocate textures and depth buffers
    texture = renderer.texturePool().createRenderTexture(size, sampler);
    resize(size);

    graphAssets.putAsset<rgi::FinalRenderTextureAsset>(
        framebuffer, viewport, scissor, clearColors, std::size(clearColors));
}

RenderTexture::~RenderTexture() {
    destroy();
    commandPool.cleanup();
}

void RenderTexture::resize(const glm::u32vec2& size) {
    if (getSize() != size) { texture->ensureSize(size); }
    depthBuffer.create(renderer.vulkanState(),
                       StandardAttachmentBuffers::findDepthFormat(renderer.vulkanState()),
                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                       {size.x, size.y});

    // set attachment sets and bind framebuffers
    VkRenderPass renderPass = renderer.renderPassCache()
                                  .getRenderPass(Config::RenderPassIds::StandardAttachmentDefault)
                                  .rawPass();
    attachmentSet.setRenderExtent(scissor.extent);
    attachmentSet.setAttachments(
        texture->getImage(), texture->getView(), depthBuffer.image(), depthBuffer.view());
    framebuffer.create(renderer.vulkanState(), renderPass, attachmentSet);
}

void RenderTexture::destroy() {
    if (texture) {
        framebuffer.deferCleanup();
        depthBuffer.deferDestroy();
        texture.release();
    }
}

void RenderTexture::render() {
    auto commandBuffer = commandPool.createBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    renderScene(commandBuffer);
    compositeSceneAndOverlay(commandBuffer);
    commandBuffer.submit();
}

void RenderTexture::Handle::release() {
    if (texture) {
        renderer->destroyRenderTexture(texture);
        texture = nullptr;
    }
}

} // namespace vk
} // namespace rc
} // namespace bl

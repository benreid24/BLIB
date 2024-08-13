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
    commandBuffers.create(renderer.vulkanState());
    attachments.emptyInit(renderer.vulkanState());
    framebuffers.emptyInit(renderer.vulkanState());

    scissor.extent.width  = size.x;
    scissor.extent.height = size.y;
    scissor.offset        = {0, 0};
    viewport.minDepth     = 0.f;
    viewport.maxDepth     = 1.f;
    viewport.x            = 0.f;
    viewport.y            = 0.f;
    viewport.width        = size.x;
    viewport.height       = size.y;

    texture = renderer.texturePool().createRenderTexture(size, sampler);
    resize(size);

    graphAssets.putAsset<rgi::FinalRenderTextureAsset>(
        framebuffers, viewport, scissor, clearColors, std::size(clearColors));
}

RenderTexture::~RenderTexture() {
    destroy();
    commandBuffers.destroy();
}

void RenderTexture::resize(const glm::u32vec2& size) {
    if (getSize() != size) { texture->resize(size); }

    scissor.extent.width  = size.x;
    scissor.extent.height = size.y;
    viewport.width        = size.x;
    viewport.height       = size.y;

    VkRenderPass renderPass = renderer.renderPassCache()
                                  .getRenderPass(Config::RenderPassIds::StandardAttachmentDefault)
                                  .rawPass();

    unsigned int i = 0;
    attachments.visit([this, size, renderPass, &i](auto& a) {
        a.depthBuffer.create(renderer.vulkanState(),
                             StandardAttachmentBuffers::findDepthFormat(renderer.vulkanState()),
                             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                             {size.x, size.y});

        a.attachmentSet.setRenderExtent(scissor.extent);
        a.attachmentSet.setAttachments(texture.asRenderTexture()->getImages().getRaw(i).image,
                                       texture.asRenderTexture()->getImages().getRaw(i).view,
                                       a.depthBuffer.image(),
                                       a.depthBuffer.view());
        ++i;
    });

    i = 0;
    framebuffers.visit([this, &i, renderPass](auto& fb) {
        fb.create(renderer.vulkanState(), renderPass, attachments.getRaw(i).attachmentSet);
        ++i;
    });
}

void RenderTexture::destroy() {
    if (texture) {
        clearScenes();
        framebuffers.cleanup([](auto& fb) { fb.deferCleanup(); });
        attachments.cleanup([](auto& a) { a.depthBuffer.deferDestroy(); });
        texture.release();
    }
}

void RenderTexture::render() {
    if (texture) {
        auto commandBuffer = commandBuffers.begin();
        renderScene(commandBuffer);
        compositeSceneAndOverlay(commandBuffer);
        commandBuffers.submit();
    }
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

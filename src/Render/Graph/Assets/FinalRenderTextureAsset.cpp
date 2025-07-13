#include <BLIB/Render/Graph/Assets/FinalRenderTextureAsset.hpp>

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FinalRenderTextureAsset::FinalRenderTextureAsset(res::TextureRef texture,
                                                 const VkViewport& viewport,
                                                 const VkRect2D& scissor,
                                                 const VkClearValue* clearColors,
                                                 const std::uint32_t clearColorCount)
: FramebufferAsset(rg::AssetTags::FinalFrameOutput, cfg::RenderPassIds::StandardAttachmentPass,
                   viewport, scissor, clearColors, clearColorCount)
, vs(nullptr)
, texture(texture) {
    addDependency(rg::AssetTags::DepthBuffer);
}

void FinalRenderTextureAsset::doCreate(engine::Engine&, Renderer& renderer, RenderTarget*) {
    vs          = &renderer.vulkanState();
    renderPass  = &renderer.renderPassCache().getRenderPass(renderPassId);
    depthBuffer = dynamic_cast<DepthBuffer*>(getDependency(0));
    if (!depthBuffer) {
        throw std::runtime_error("FinalSwapframeAsset requires a DepthBuffer dependency");
    }
    depthBuffer->setSizeMode(DepthBuffer::Target);

    attachmentSet.setRenderExtent(scissor.extent);
    attachmentSet.setAttachmentCount(2);
    attachmentSet.setAttachment(0, texture->getImage(), texture->getView());
    attachmentSet.setAttachment(1, depthBuffer->getBuffer());
    attachmentSet.setAttachmentAspect(0, VK_IMAGE_ASPECT_COLOR_BIT);
    attachmentSet.setAttachmentAspect(1, VK_IMAGE_ASPECT_DEPTH_BIT);
    framebuffer.create(renderer.vulkanState(), renderPass, attachmentSet);
}

void FinalRenderTextureAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop, handled by render pass
}

void FinalRenderTextureAsset::doStartOutput(const rg::ExecutionContext& ctx) {
    beginRender(ctx.commandBuffer, true);
}

void FinalRenderTextureAsset::doEndOutput(const rg::ExecutionContext& ctx) {
    finishRender(ctx.commandBuffer);
}

vk::Framebuffer& FinalRenderTextureAsset::currentFramebuffer() { return framebuffer; }

vk::Framebuffer& FinalRenderTextureAsset::getFramebuffer(std::uint32_t) { return framebuffer; }

void FinalRenderTextureAsset::onResize(glm::u32vec2) {
    depthBuffer->ensureValid({scissor.extent.width, scissor.extent.height}, VK_SAMPLE_COUNT_1_BIT);
    attachmentSet.setRenderExtent(scissor.extent);
    attachmentSet.setAttachment(0, texture->getImage(), texture->getView());
    attachmentSet.setAttachment(1, depthBuffer->getBuffer());
    framebuffer.create(*vs, renderPass, attachmentSet);
}

} // namespace rgi
} // namespace rc
} // namespace bl

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

    attachmentSet.setRenderExtent(scissor.extent);
    attachmentSet.setAttachments(texture->getImage(),
                                 texture->getView(),
                                 depthBuffer->getBuffer().getImage(),
                                 depthBuffer->getBuffer().getView());
    framebuffer.create(renderer.vulkanState(), renderPass->rawPass(), attachmentSet);
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
    attachmentSet.setRenderExtent(scissor.extent);
    attachmentSet.setAttachments(texture->getImage(),
                                 texture->getView(),
                                 depthBuffer->getBuffer().getImage(),
                                 depthBuffer->getBuffer().getView());
    framebuffer.create(*vs, renderPass->rawPass(), attachmentSet);
}

} // namespace rgi
} // namespace rc
} // namespace bl

#include <BLIB/Render/Graph/Assets/FinalRenderTextureAsset.hpp>

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FinalRenderTextureAsset::FinalRenderTextureAsset(vk::Framebuffer& framebuffer,
                                                 const VkViewport& viewport,
                                                 const VkRect2D& scissor,
                                                 const VkClearValue* clearColors,
                                                 const std::uint32_t clearColorCount)
: FramebufferAsset(rg::AssetTags::FinalFrameOutput, cfg::RenderPassIds::StandardAttachmentDefault,
                   viewport, scissor, clearColors, clearColorCount)
, framebuffer(framebuffer) {}

void FinalRenderTextureAsset::doCreate(engine::Engine&, Renderer& renderer, RenderTarget*) {
    renderPass = &renderer.renderPassCache().getRenderPass(renderPassId);
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
    // noop
}

} // namespace rgi
} // namespace rc
} // namespace bl

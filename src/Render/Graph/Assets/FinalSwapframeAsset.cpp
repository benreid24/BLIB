#include <BLIB/Render/Graph/Assets/FinalSwapframeAsset.hpp>

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FinalSwapframeAsset::FinalSwapframeAsset(vk::PerSwapFrame<vk::Framebuffer>& framebuffers,
                                         const VkViewport& viewport, const VkRect2D& scissor,
                                         const VkClearValue* clearColors,
                                         const std::uint32_t clearColorCount)
: FramebufferAsset(rg::AssetTags::FinalFrameOutput, cfg::RenderPassIds::SwapchainPass, viewport,
                   scissor, clearColors, clearColorCount)
, framebuffers(framebuffers) {}

void FinalSwapframeAsset::doCreate(engine::Engine&, Renderer& renderer, RenderTarget*) {
    renderPass = &renderer.renderPassCache().getRenderPass(renderPassId);
}

void FinalSwapframeAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop, handled by render pass
}

void FinalSwapframeAsset::doStartOutput(const rg::ExecutionContext& ctx) {
    beginRender(ctx.commandBuffer, true);
}

void FinalSwapframeAsset::doEndOutput(const rg::ExecutionContext& ctx) {
    finishRender(ctx.commandBuffer);
}

vk::Framebuffer& FinalSwapframeAsset::currentFramebuffer() { return framebuffers.current(); }

vk::Framebuffer& FinalSwapframeAsset::getFramebuffer(std::uint32_t i) {
    return framebuffers.getRaw(i);
}

void FinalSwapframeAsset::onResize(glm::u32vec2) {
    // noop
}

} // namespace rgi
} // namespace rc
} // namespace bl

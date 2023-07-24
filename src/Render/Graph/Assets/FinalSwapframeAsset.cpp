#include <BLIB/Render/Graph/Assets/FinalSwapframeAsset.hpp>

#include <BLIB/Render/Config.hpp>
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
: FramebufferAsset(rg::AssetTags::FinalFrameOutput, Config::RenderPassIds::SwapchainDefault,
                   viewport, scissor, clearColors, clearColorCount)
, framebuffers(framebuffers) {}

void FinalSwapframeAsset::doCreate(engine::Engine&, Renderer& renderer) {
    renderPass = &renderer.renderPassCache().getRenderPass(renderPassId);
}

void FinalSwapframeAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop, handled by renderpass
}

void FinalSwapframeAsset::doPrepareForOutput(const rg::ExecutionContext&) {
    // noop, handled by renderpass
}

vk::Framebuffer& FinalSwapframeAsset::currentFramebuffer() { return framebuffers.current(); }

void FinalSwapframeAsset::onResize(glm::u32vec2) {
    // noop
}

} // namespace rgi
} // namespace rc
} // namespace bl

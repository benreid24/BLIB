#include <BLIB/Render/Graph/Assets/FinalRenderTextureAsset.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FinalRenderTextureAsset::FinalRenderTextureAsset(vk::PerFrame<vk::Framebuffer>& framebuffers,
                                                 const VkViewport& viewport,
                                                 const VkRect2D& scissor,
                                                 const VkClearValue* clearColors,
                                                 const std::uint32_t clearColorCount)
: FramebufferAsset(rg::AssetTags::FinalFrameOutput,
                   Config::RenderPassIds::StandardAttachmentDefault, viewport, scissor, clearColors,
                   clearColorCount)
, framebuffers(framebuffers) {}

void FinalRenderTextureAsset::doCreate(engine::Engine&, Renderer& renderer, RenderTarget*) {
    renderPass = &renderer.renderPassCache().getRenderPass(renderPassId);
}

void FinalRenderTextureAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop, handled by renderpass
}

void FinalRenderTextureAsset::doPrepareForOutput(const rg::ExecutionContext&) {
    // noop, handled by renderpass
}

vk::Framebuffer& FinalRenderTextureAsset::currentFramebuffer() { return framebuffers.current(); }

void FinalRenderTextureAsset::onResize(glm::u32vec2) {
    // noop
}

} // namespace rgi
} // namespace rc
} // namespace bl

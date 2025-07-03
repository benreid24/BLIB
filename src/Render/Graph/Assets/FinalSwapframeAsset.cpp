#include <BLIB/Render/Graph/Assets/FinalSwapframeAsset.hpp>

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FinalSwapframeAsset::FinalSwapframeAsset(const VkViewport& viewport, const VkRect2D& scissor,
                                         const VkClearValue* clearColors,
                                         const std::uint32_t clearColorCount)
: FramebufferAsset(rg::AssetTags::FinalFrameOutput, cfg::RenderPassIds::SwapchainPass, viewport,
                   scissor, clearColors, clearColorCount) {
    addDependency(rg::AssetTags::DepthBuffer);
}

void FinalSwapframeAsset::doCreate(engine::Engine&, Renderer& renderer, RenderTarget*) {
    renderPass = &renderer.renderPassCache().getRenderPass(renderPassId);
    swapchain  = &renderer.vulkanState().swapchain;

    depthBufferAsset = dynamic_cast<DepthBuffer*>(getDependency(0));
    if (!depthBufferAsset) {
        throw std::runtime_error("FinalSwapframeAsset requires a DepthBuffer dependency");
    }

    unsigned int i = 0;
    attachmentSets.init(
        renderer.vulkanState().swapchain, [this, &renderer, &i](vk::StandardAttachmentSet& set) {
            set.setRenderExtent(scissor.extent);
            set.setAttachments(renderer.vulkanState().swapchain.swapFrameAtIndex(i).getImage(0),
                               renderer.vulkanState().swapchain.swapFrameAtIndex(i).getImageView(0),
                               depthBufferAsset->getBuffer().getImage(),
                               depthBufferAsset->getBuffer().getView());
            ++i;
        });

    i = 0;
    framebuffers.init(renderer.vulkanState().swapchain, [this, &renderer, &i](vk::Framebuffer& fb) {
        fb.create(renderer.vulkanState(), renderPass->rawPass(), attachmentSets.getRaw(i));
        ++i;
    });
}

void FinalSwapframeAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop, handled by render pass
}

void FinalSwapframeAsset::doStartOutput(const rg::ExecutionContext& ctx) {
    // TODO - consider events for swapchain invalidation
    attachmentSets.current().setAttachments(
        swapchain->swapFrameAtIndex(swapchain->currentIndex()).getImage(0),
        swapchain->swapFrameAtIndex(swapchain->currentIndex()).getImageView(0),
        depthBufferAsset->getBuffer().getImage(),
        depthBufferAsset->getBuffer().getView());
    framebuffers.current().recreateIfChanged(attachmentSets.current());
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
    attachmentSets.current().setRenderExtent(scissor.extent);
}

} // namespace rgi
} // namespace rc
} // namespace bl

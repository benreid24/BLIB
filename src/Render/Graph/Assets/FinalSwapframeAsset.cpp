#include <BLIB/Render/Graph/Assets/FinalSwapframeAsset.hpp>

#include <BLIB/Engine/Engine.hpp>
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
                   scissor, clearColors, clearColorCount)
, engine(nullptr) {
    addDependency(rg::AssetTags::DepthBuffer);
    setShouldClearOnRestart(true);
}

void FinalSwapframeAsset::doCreate(engine::Engine& e, Renderer& renderer, RenderTarget*) {
    engine     = &e;
    renderPass = &renderer.renderPassCache().getRenderPass(renderPassId);
    swapchain  = &renderer.vulkanState().swapchain;

    depthBufferAsset = dynamic_cast<DepthBuffer*>(getDependency(0));
    if (!depthBufferAsset) {
        throw std::runtime_error("FinalSwapframeAsset requires a DepthBuffer dependency");
    }

    const auto size = engine->window().getSfWindow().getSize();
    unsigned int i  = 0;
    attachmentSets.init(renderer.vulkanState().swapchain,
                        [this, &i, &size](vk::AttachmentSet& set) {
                            set.setRenderExtent({size.x, size.y});
                            updateAttachments(i);
                            set.setAttachmentAspect(0, VK_IMAGE_ASPECT_COLOR_BIT);
                            set.setAttachmentAspect(1, VK_IMAGE_ASPECT_DEPTH_BIT);
                            set.setAttachmentAspect(2, VK_IMAGE_ASPECT_COLOR_BIT);
                            ++i;
                        });

    i = 0;
    framebuffers.init(renderer.vulkanState().swapchain, [this, &renderer, &i](vk::Framebuffer& fb) {
        fb.create(renderer.vulkanState(), renderPass, attachmentSets.getRaw(i));
        ++i;
    });

    onResize({});
}

void FinalSwapframeAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop, handled by render pass
}

void FinalSwapframeAsset::doStartOutput(const rg::ExecutionContext& ctx) {
    // TODO - consider events for swapchain invalidation
    updateAttachments(engine->renderer().vulkanState().swapchain.currentIndex());
    framebuffers.current().recreateIfChanged(attachmentSets.current());
    beginRender(ctx.commandBuffer, true);
    setShouldClearOnRestart(false);
}

void FinalSwapframeAsset::doEndOutput(const rg::ExecutionContext& ctx) {
    finishRender(ctx.commandBuffer);
}

vk::Framebuffer& FinalSwapframeAsset::currentFramebuffer() { return framebuffers.current(); }

vk::Framebuffer& FinalSwapframeAsset::getFramebuffer(std::uint32_t i) {
    return framebuffers.getRaw(i);
}

void FinalSwapframeAsset::onResize(glm::u32vec2) {
    if (engine) {
        const auto size = engine->window().getSfWindow().getSize();
        attachmentSets.visit(
            [this, &size](vk::AttachmentSet& set) { set.setRenderExtent({size.x, size.y}); });
        if (engine->renderer().getSettings().getAntiAliasing() != Settings::AntiAliasing::None) {
            sampledImage.resize({size.x, size.y}, false);
        }
        depthBufferAsset->onResize({size.x, size.y});
        updateAllAttachments();
    }
}

void FinalSwapframeAsset::onReset() { setShouldClearOnRestart(true); }

void FinalSwapframeAsset::updateAttachments(std::uint32_t i) {
    const bool useMsaa =
        engine->renderer().getSettings().getAntiAliasing() != Settings::AntiAliasing::None;
    auto& set                          = attachmentSets.getRaw(i);
    auto& chain                        = engine->renderer().vulkanState().swapchain;
    const unsigned int swapIndex       = useMsaa ? 2 : 0;
    const unsigned int attachmentCount = useMsaa ? 3 : 2;

    std::array<VkImage, 3> images{};
    std::array<VkImageView, 3> views{};
    const std::array<VkImageAspectFlags, 3> aspects = {
        VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_COLOR_BIT};

    set.setAttachment(swapIndex,
                      chain.swapFrameAtIndex(i).getImage(0),
                      chain.swapFrameAtIndex(i).getImageView(0));
    set.setAttachment(1, depthBufferAsset->getBuffer());
    if (useMsaa) { set.setAttachment(0, sampledImage); }
    set.setAttachmentCount(attachmentCount);
    set.setRenderExtent(chain.swapFrameAtIndex(i).getRenderExtent());
}

void FinalSwapframeAsset::updateAllAttachments() {
    unsigned int i = 0;
    attachmentSets.visit([this, &i](vk::AttachmentSet&) { updateAttachments(i++); });
}

} // namespace rgi
} // namespace rc
} // namespace bl

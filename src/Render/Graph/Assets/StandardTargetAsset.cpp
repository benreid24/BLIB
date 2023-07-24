#include <BLIB/Render/Graph/Assets/StandardTargetAsset.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
StandardTargetAsset::StandardTargetAsset(std::uint32_t renderPassId, const VkViewport& viewport,
                                         const VkRect2D& scissor, const VkClearValue* clearColors,
                                         const std::uint32_t clearColorCount)
: FramebufferAsset(rg::AssetTags::RenderedSceneOutput, renderPassId, viewport, scissor, clearColors,
                   clearColorCount)
, renderer(nullptr) {}

void StandardTargetAsset::doCreate(engine::Engine&, Renderer& r) {
    renderer = &r;
    images.emptyInit(r.vulkanState());
    framebuffers.emptyInit(r.vulkanState());
    renderPass = &renderer->renderPassCache().getRenderPass(renderPassId);
}

void StandardTargetAsset::doPrepareForInput(const rg::ExecutionContext& context) {
    // noop, handled by render pass
}

void StandardTargetAsset::doPrepareForOutput(const rg::ExecutionContext& context) {
    // noop, handled by render pass
}

void StandardTargetAsset::onResize(glm::u32vec2 newSize) {
    if (renderer) {
        images.init(renderer->vulkanState(), [this, newSize](vk::StandardAttachmentBuffers& image) {
            image.create(renderer->vulkanState(), {newSize.x, newSize.y});
        });
        unsigned int i = 0;
        framebuffers.init(renderer->vulkanState(), [this, &i](vk::Framebuffer& fb) {
            fb.create(
                renderer->vulkanState(), renderPass->rawPass(), images.getRaw(i).attachmentSet());
            ++i;
        });
    }
}

vk::Framebuffer& StandardTargetAsset::currentFramebuffer() { return framebuffers.current(); }

} // namespace rgi
} // namespace rc
} // namespace bl

#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FramebufferAsset::FramebufferAsset(std::string_view tag, std::uint32_t renderPassId,
                                   const VkViewport& viewport, const VkRect2D& scissor,
                                   const VkClearValue* clearColors,
                                   const std::uint32_t clearColorCount)
: Asset(tag)
, renderPassId(renderPassId)
, viewport(viewport)
, scissor(scissor)
, renderPass(nullptr)
, clearColors(clearColors)
, clearColorCount(clearColorCount)
, clearOnRestart(false)
, startCount(0) {}

void FramebufferAsset::setShouldClearOnRestart(bool c) { clearOnRestart = c; }

void FramebufferAsset::beginRender(VkCommandBuffer commandBuffer, bool setViewport) {
    currentFramebuffer().beginRender(commandBuffer,
                                     scissor,
                                     clearColors,
                                     clearColorCount,
                                     setViewport,
                                     renderPass ? renderPass->rawPass() : nullptr,
                                     clearOnRestart || startCount == 0);
    ++startCount;
}

void FramebufferAsset::finishRender(VkCommandBuffer commandBuffer) {
    currentFramebuffer().finishRender(commandBuffer);
    --startCount;
}

std::array<const vk::AttachmentSet*, cfg::Limits::MaxConcurrentFrames>
FramebufferAsset::getAttachmentSets() {
    std::array<const vk::AttachmentSet*, cfg::Limits::MaxConcurrentFrames> sets;
    for (std::uint32_t i = 0; i < cfg::Limits::MaxConcurrentFrames; ++i) {
        sets[i] = &getFramebuffer(i).getAttachmentSet();
    }
    return sets;
}

} // namespace rgi
} // namespace rc
} // namespace bl

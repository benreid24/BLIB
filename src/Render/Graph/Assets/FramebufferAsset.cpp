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
, cachedSize(0, 0) {}

void FramebufferAsset::notifyResize(glm::u32vec2 newSize) {
    if (newSize != cachedSize) {
        cachedSize = newSize;
        onResize(newSize);
    }
}

} // namespace rgi
} // namespace rc
} // namespace bl

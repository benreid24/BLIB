#include <BLIB/Render/Overlays/OverlayRenderContext.hpp>

#include <BLIB/Render/Vulkan/VkCheck.hpp>

namespace bl
{
namespace render
{
namespace overlay
{
OverlayRenderContext::OverlayRenderContext(VkCommandBuffer cb)
: commandBuffer(cb)
, currentPipeline(nullptr) {}

void OverlayRenderContext::bindPipeline(VkPipeline pipeline) {
    if (currentPipeline != pipeline) {
        currentPipeline = pipeline;
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }
}

} // namespace overlay
} // namespace render
} // namespace bl

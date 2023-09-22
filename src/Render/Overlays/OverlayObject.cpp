#include <BLIB/Render/Overlays/OverlayObject.hpp>

namespace bl
{
namespace rc
{
namespace ovy
{
OverlayObject::OverlayObject()
: SceneObject()
, overlay(nullptr)
, descriptorCount(0) {}

void OverlayObject::refreshViewport(Viewport* viewport, const VkViewport& global,
                                    const VkViewport& parent) {
    // TODO - pass down viewport from dynamic fetch
    cachedViewport = viewport ? viewport->createViewport(global, parent) : parent;
    cachedScissor  = ovy::Viewport::viewportToScissor(cachedViewport);
}

void OverlayObject::applyViewport(VkCommandBuffer commandBuffer) {
    vkCmdSetViewport(commandBuffer, 0, 1, &cachedViewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &cachedScissor);
}

} // namespace ovy
} // namespace rc
} // namespace bl

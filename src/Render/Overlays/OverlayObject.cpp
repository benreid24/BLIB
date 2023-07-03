#include <BLIB/Render/Overlays/OverlayObject.hpp>

namespace bl
{
namespace gfx
{
namespace ovy
{
OverlayObject::OverlayObject()
: SceneObject()
, descriptorCount(0) {}

void OverlayObject::registerChild(scene::Key cid) {
    if (children.size() == children.capacity()) {
        children.reserve(std::max(children.capacity() * 2, static_cast<std::size_t>(8)));
    }
    children.emplace_back(cid);
}

void OverlayObject::removeChild(scene::Key cid) {
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (*it == cid) {
            children.erase(it);
            return;
        }
    }
}

void OverlayObject::refreshViewport(const VkViewport& global, const VkViewport& parent) {
    cachedViewport = viewport.valid() ? viewport.get().createViewport(global, parent) : parent;
    cachedScissor  = ovy::Viewport::viewportToScissor(cachedViewport);
}

void OverlayObject::applyViewport(VkCommandBuffer commandBuffer) {
    vkCmdSetViewport(commandBuffer, 0, 1, &cachedViewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &cachedScissor);
}

} // namespace ovy
} // namespace gfx
} // namespace bl

#include <BLIB/Components/BatchedSlideshows.hpp>

namespace bl
{
namespace com
{
BatchedSlideshows::BatchedSlideshows(rc::vk::VulkanState& vulkanState, unsigned int ic) {
    create(vulkanState, ic);
}

void BatchedSlideshows::create(rc::vk::VulkanState& vulkanState, unsigned int ic) {
    indexBuffer.create(vulkanState, ic * 4, ic * 6);
    drawParams = indexBuffer.getDrawParameters();
}

void BatchedSlideshows::updateDrawParams() {
    if (indexBuffer.getIndexBuffer().vertexCount() > 0) {
        drawParams = indexBuffer.getDrawParameters();
        if (sceneRef.object) { syncDrawParamsToScene(); }
    }
}

} // namespace com
} // namespace bl

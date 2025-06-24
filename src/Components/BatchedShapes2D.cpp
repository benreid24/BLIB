#include <BLIB/Components/BatchedShapes2D.hpp>

namespace bl
{
namespace com
{
BatchedShapes2D::BatchedShapes2D(rc::vk::VulkanState& vulkanState, std::uint32_t vertexCapacity,
                                 std::uint32_t indexCapacity) {
    indexBuffer.create(vulkanState, vertexCapacity, indexCapacity);
}

void BatchedShapes2D::commit() { drawParams = indexBuffer.getDrawParameters(); }

} // namespace com
} // namespace bl

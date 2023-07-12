#include <BLIB/Components/Mesh.hpp>

namespace bl
{
namespace com
{
void Mesh::create(rc::vk::VulkanState& vulkanState, std::uint32_t vertexCount,
                  std::uint32_t indexCount) {
    gpuBuffer.create(vulkanState, vertexCount, indexCount);
    drawParams = gpuBuffer.getDrawParameters();
}

} // namespace com
} // namespace bl

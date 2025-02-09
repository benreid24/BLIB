#include <BLIB/Components/Mesh.hpp>

namespace bl
{
namespace com
{
void Mesh::create(rc::vk::VulkanState& vulkanState, std::uint32_t vertexCount,
                  std::uint32_t indexCount) {
    gpuBuffer.create(vulkanState, vertexCount, indexCount);
    drawParams = gpuBuffer.getDrawParameters();
    gpuBuffer.queueTransfer();
}

void Mesh::create(rc::vk::VulkanState& vs, std::vector<rc::prim::Vertex3D>&& vertices,
                  std::vector<std::uint32_t>&& indices) {
    gpuBuffer.create(vs, std::move(vertices), std::move(indices));
    drawParams = gpuBuffer.getDrawParameters();
    gpuBuffer.queueTransfer();
}

} // namespace com
} // namespace bl

#include <BLIB/Render/Components/Mesh.hpp>

namespace bl
{
namespace gfx
{
namespace com
{
void Mesh::create(vk::VulkanState& vulkanState, std::uint32_t vertexCount,
                  std::uint32_t indexCount) {
    gpuBuffer.create(vulkanState, vertexCount, indexCount);
    drawParams = gpuBuffer.getDrawParameters();
}

} // namespace com
} // namespace gfx
} // namespace bl

#include <BLIB/Render/Components/Mesh.hpp>

namespace bl
{
namespace render
{
namespace com
{
void Mesh::create(VulkanState& vulkanState, std::uint32_t vertexCount, std::uint32_t indexCount) {
    vertices.resize(vertexCount);
    indices.resize(indexCount);
    gpuBuffer.create(vulkanState, vertexCount, indexCount);
    gpuBuffer.vertices().configureWrite(vertices.data(), 0, gpuBuffer.vertices().size());
    gpuBuffer.indices().configureWrite(indices.data(), 0, gpuBuffer.indices().size());
    drawParams = gpuBuffer.getDrawParameters();
}

} // namespace com
} // namespace render
} // namespace bl

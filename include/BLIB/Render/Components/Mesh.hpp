#ifndef BLIB_RENDER_COMPONENTS_MESH_HPP
#define BLIB_RENDER_COMPONENTS_MESH_HPP

#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Primitives/IndexBuffer.hpp>
#include <vector>

namespace bl
{
namespace render
{
namespace com
{
/**
 * @brief ECS component for a basic mesh from an index buffer
 *
 * @ingroup Renderer
 */
struct Mesh : public DrawableBase {
    std::vector<prim::Vertex> vertices;
    std::vector<std::uint32_t> indices;
    prim::IndexBuffer gpuBuffer;

    /**
     * @brief Helper method to initialize all the mesh data
     *
     * @param vulkanState Renderer Vulkan state
     * @param vertexCount Number of vertices to create
     * @param indexCount Number of indices to create
     */
    void create(VulkanState& vulkanState, std::uint32_t vertexCount, std::uint32_t indexCount);
};

} // namespace com
} // namespace render
} // namespace bl

#endif

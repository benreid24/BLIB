#ifndef BLIB_RENDER_COMPONENTS_MESH_HPP
#define BLIB_RENDER_COMPONENTS_MESH_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <vector>

namespace bl
{
namespace gfx
{
namespace com
{
/**
 * @brief ECS component for a basic mesh from an index buffer
 *
 * @ingroup Renderer
 */
struct Mesh : public DrawableBase {
    buf::IndexBuffer gpuBuffer;

    /**
     * @brief Helper method to initialize all the mesh data
     *
     * @param vulkanState Renderer Vulkan state
     * @param vertexCount Number of vertices to create
     * @param indexCount Number of indices to create
     */
    void create(vk::VulkanState& vulkanState, std::uint32_t vertexCount, std::uint32_t indexCount);
};

} // namespace com
} // namespace gfx
} // namespace bl

#endif

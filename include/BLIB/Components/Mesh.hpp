#ifndef BLIB_COMPONENTS_MESH_HPP
#define BLIB_COMPONENTS_MESH_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <vector>

namespace bl
{
/// Collection of built-in ECS components
namespace com
{
/**
 * @brief ECS component for a basic mesh from an index buffer
 *
 * @ingroup Components
 */
struct Mesh : public rc::rcom::DrawableBase {
    rc::buf::IndexBuffer gpuBuffer;

    /**
     * @brief Helper method to initialize all the mesh data
     *
     * @param vulkanState Renderer Vulkan state
     * @param vertexCount Number of vertices to create
     * @param indexCount Number of indices to create
     */
    void create(rc::vk::VulkanState& vulkanState, std::uint32_t vertexCount,
                std::uint32_t indexCount);
};

} // namespace com
} // namespace bl

#endif

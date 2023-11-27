#ifndef BLIB_COMPONENTS_BATCHEDSHAPES2D_HPP
#define BLIB_COMPONENTS_BATCHEDSHAPES2D_HPP

#include <BLIB/Render/Buffers/BatchIndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component that allows the renderer to render a batch of 2d shapes
 *
 * @ingroup Components
 */
struct BatchedShapes2D : public rc::rcom::DrawableBase {
    rc::buf::BatchIndexBuffer indexBuffer;

    /**
     * @brief Creates the index buffer with the given starting capacities
     *
     * @param vulkanState The engine renderer Vulkan state
     * @param vertexCapacity The number of vertices to allocate
     * @param indexCapacity The number of indices to allocate
     */
    BatchedShapes2D(rc::vk::VulkanState& vulkanState, std::uint32_t vertexCapacity,
                    std::uint32_t indexCapacity);

    /**
     * @brief Updates the draw parameters and syncs with the scene
     */
    void commit();
};

} // namespace com
} // namespace bl

#endif

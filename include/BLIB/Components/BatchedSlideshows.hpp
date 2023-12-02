#ifndef BLIB_COMPONENTS_BATCHEDSLIDESHOWS_HPP
#define BLIB_COMPONENTS_BATCHEDSLIDESHOWS_HPP

#include <BLIB/Render/Buffers/BatchIndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Primitives/SlideshowVertex.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;
}
} // namespace rc

namespace com
{
/**
 * @brief Drawable component for batches of 2d slideshow animations
 *
 * @ingroup Components
 */
struct BatchedSlideshows : public rc::rcom::DrawableBase {
    rc::buf::BatchIndexBufferT<rc::prim::SlideshowVertex> indexBuffer;

    /**
     * @brief Creates the index buffer to render the slideshows
     *
     * @param vulkanState Renderer Vulkan state
     * @param initialCapacity The number of slideshows to create vertices for
     */
    BatchedSlideshows(rc::vk::VulkanState& vulkanState, unsigned int initialCapacity);

    /**
     * @brief Creates the index buffer to render the slideshows
     *
     * @param vulkanState Renderer Vulkan state
     * @param initialCapacity The number of slideshows to create vertices for
     */
    void create(rc::vk::VulkanState& vulkanState, unsigned int initialCapacity);

    /**
     * @brief Updates and commits the draw parameters
     */
    void updateDrawParams();
};

} // namespace com
} // namespace bl

#endif

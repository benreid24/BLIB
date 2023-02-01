#ifndef BLIB_RENDER_VULKAN_STANDARDIMAGEBUFFER_HPP
#define BLIB_RENDER_VULKAN_STANDARDIMAGEBUFFER_HPP

#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
/**
 * @brief Resource class that can create renderable target images that are compatible with most
 *        rendering flows. Contains a depth buffer and color attachment
 *
 * @ingroup Renderer
 */
class StandardImageBuffer {
public:
    /**
     * @brief Creates an empty render buffer with no resources
     */
    StandardImageBuffer();

    /**
     * @brief Frees resources if not already destroyed
     */
    ~StandardImageBuffer();

    /**
     * @brief Frees prior images, if any, and creates a new color and depth image
     *
     * @param vulkaState Renderer vulkan state
     * @param size The size of the color and depth attachments to create
     */
    void create(VulkanState& vulkaState, const VkExtent2D& size);

    /**
     * @brief Destroys the images, views, sampler, and frees GPU memory
     */
    void destroy();

    /**
     * @brief Performs a layout transition and inserts a barrier to ensure that the buffers are
     *        ready to be rendered to before draw commands are executed. This does not synchronize
     *        whether or not the image is done being sampled from. That must be handled externally
     *
     * @param commandBuffer Command buffer to record into
     */
    void prepareForRendering(VkCommandBuffer commandBuffer);

    /**
     * @brief Call after rendering is completed. This records a barrier to block and transition to a
     *        layout suitable for sampling
     *
     * @param commandBuffer Command buffer to record to
     */
    void prepareForSampling(VkCommandBuffer commandBuffer);

    /**
     * @brief Returns the attachment set to use to render to this target
     */
    constexpr const StandardAttachmentSet& attachmentSet() const;

    /**
     * @brief Returns the size of the images contained in the buffer
     */
    constexpr const VkExtent2D& bufferSize() const;

private:
    VulkanState* owner;
    StandardAttachmentSet attachments;
    VkDeviceMemory gpuMemory;
    VkImageLayout currentColorLayout;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const StandardAttachmentSet& StandardImageBuffer::attachmentSet() const {
    return attachments;
}

inline constexpr const VkExtent2D& StandardImageBuffer::bufferSize() const {
    return attachments.extent;
}

} // namespace render
} // namespace bl

#endif

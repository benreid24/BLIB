#ifndef BLIB_RENDER_VULKAN_STANDARDIMAGEBUFFER_HPP
#define BLIB_RENDER_VULKAN_STANDARDIMAGEBUFFER_HPP

#include <BLIB/Render/Vulkan/AttachmentBuffer.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
/**
 * @brief Resource class that can create renderable target images that are compatible with most
 *        rendering flows. Contains a depth buffer and color attachment
 *
 * @ingroup Renderer
 */
class StandardAttachmentBuffers {
public:
    static constexpr VkFormat DefaultColorFormat = VK_FORMAT_R8G8B8A8_SRGB;

    /**
     * @brief Creates an empty render buffer with no resources
     */
    StandardAttachmentBuffers();

    /**
     * @brief Frees resources if not already destroyed
     */
    ~StandardAttachmentBuffers();

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
     * @brief Returns the attachment set to use to render to this target
     */
    constexpr const StandardAttachmentSet& attachmentSet() const;

    /**
     * @brief Returns the size of the images contained in the buffer
     */
    constexpr const VkExtent2D& bufferSize() const;

    /**
     * @brief Helper function find the proper format to use for the depth buffer
     *
     * @param vulkanState Renderer Vulkan state
     * @return The format to use for the depth attachment
     */
    static VkFormat findDepthFormat(VulkanState& vulkanState);

private:
    VulkanState* owner;
    StandardAttachmentSet attachments;
    AttachmentBuffer colorAttachment;
    AttachmentBuffer depthAttachment;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const StandardAttachmentSet& StandardAttachmentBuffers::attachmentSet() const {
    return attachments;
}

inline constexpr const VkExtent2D& StandardAttachmentBuffers::bufferSize() const {
    return attachments.renderExtent();
}

} // namespace vk
} // namespace gfx
} // namespace bl

#endif

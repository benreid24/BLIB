#ifndef BLIB_RENDER_VULKAN_SWAPDEPTHBUFFERS_HPP
#define BLIB_RENDER_VULKAN_SWAPDEPTHBUFFERS_HPP

#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Utility class to create and manage an image attachment
 *
 * @ingroup Renderer
 */
class AttachmentBuffer {
public:
    /**
     * @brief Creates an uninitialized attachment buffer
     */
    AttachmentBuffer();

    /**
     * @brief Destroys the attachment buffer
     */
    ~AttachmentBuffer();

    /**
     * @brief Creates the attachment buffer. Noop if extent is unchanged
     *
     * @param vulkanState Renderer Vulkan state
     * @param format The image format to create with
     * @param usage What the attachment will be used for
     * @param extent The size of the attachment buffer to create
     */
    void create(VulkanState& vulkanState, VkFormat format, VkImageUsageFlags usage,
                const VkExtent2D& extent);

    /**
     * @brief Clears the attachment buffer and transitions it to
     *        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
     *
     * @param aspect The image aspect
     * @param clearColor The color to clear to
     */
    void clearAndPrepareForSampling(VkImageAspectFlags aspect    = VK_IMAGE_ASPECT_COLOR_BIT,
                                    VkClearColorValue clearColor = {{0.f, 0.f, 0.f, 1.f}});

    /**
     * @brief Destroys the attachment buffer
     */
    void destroy();

    /**
     * @brief Queues destruction for later
     */
    void deferDestroy();

    /**
     * @brief The Vulkan handle to the image of the attachment buffer
     */
    VkImage image() const;

    /**
     * @brief Vulkan handle to the view of the attachment buffer
     */
    VkImageView view() const;

private:
    VulkanState* vulkanState;
    VkExtent2D size;
    VmaAllocation alloc;
    VkImage imageHandle;
    VkImageView viewHandle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkImage AttachmentBuffer::image() const { return imageHandle; }

inline VkImageView AttachmentBuffer::view() const { return viewHandle; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif

#ifndef BLIB_RENDER_VULKAN_SWAPDEPTHBUFFERS_HPP
#define BLIB_RENDER_VULKAN_SWAPDEPTHBUFFERS_HPP

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

namespace bl
{
namespace render
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
     * @brief Destroys the attachment buffer
     */
    void destroy();

    /**
     * @brief The Vulkan handle to the image of the attachment buffer
     */
    constexpr VkImage image() const;

    /**
     * @brief Vulkan handle to the view of the attachment buffer
     */
    constexpr VkImageView view() const;

private:
    VulkanState* vulkanState;
    VkExtent2D size;
    VmaAllocation alloc;
    VkImage imageHandle;
    VkImageView viewHandle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkImage AttachmentBuffer::image() const { return imageHandle; }

inline constexpr VkImageView AttachmentBuffer::view() const { return viewHandle; }

} // namespace vk
} // namespace render
} // namespace bl

#endif

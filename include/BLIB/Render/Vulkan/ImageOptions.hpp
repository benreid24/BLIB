#ifndef BLIB_RENDER_VULKAN_IMAGEOPTIONS_HPP
#define BLIB_RENDER_VULKAN_IMAGEOPTIONS_HPP

#include <BLIB/Render/Vulkan/CommonTextureFormats.hpp>
#include <BLIB/Vulkan.hpp>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Basic POD struct for image creation
 *
 * @ingroup Renderer
 */
struct ImageOptions {
    /**
     * @brief The type of image that this is. Also hints usage
     */
    enum struct Type { Image2D, Cubemap };

    Type type                            = Type::Image2D;
    VkFormat format                      = CommonTextureFormats::SRGBA32Bit;
    VkImageUsageFlags usage              = 0;
    VkExtent2D extent                    = {0, 0};
    VkImageAspectFlags aspect            = VK_IMAGE_ASPECT_COLOR_BIT;
    VmaAllocationCreateFlags allocFlags  = 0;
    VkMemoryPropertyFlags memoryLocation = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkImageCreateFlags extraCreateFlags  = 0;
    VkImageAspectFlags viewAspect        = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
    VkSampleCountFlagBits samples        = VK_SAMPLE_COUNT_1_BIT;
    std::uint32_t mipLevels              = 1;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

#ifndef BLIB_RENDER_VULKAN_SWAPFRAME_HPP
#define BLIB_RENDER_VULKAN_SWAPFRAME_HPP

#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace render
{
struct SwapFrame {
    VkImage image;
    VkImageView imageView;
};

} // namespace render
} // namespace bl

#endif
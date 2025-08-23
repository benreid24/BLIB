#ifndef BLIB_RENDER_VULKAN_COMMONTEXTUREFORMATS_HPP
#define BLIB_RENDER_VULKAN_COMMONTEXTUREFORMATS_HPP

#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Contains common specific formats
 *
 * @ingroup Renderer
 */
struct CommonTextureFormats {
    /// 8-bit unsigned integer, 4 channels, linear
    static constexpr VkFormat LinearRGBA32Bit = VK_FORMAT_R8G8B8A8_UNORM;

    /// 8-bit unsigned integer, 4 channels, sRGB
    static constexpr VkFormat SRGBA32Bit = VK_FORMAT_R8G8B8A8_SRGB;

    /// 16 bit signed float, linear
    static constexpr VkFormat HDRColor = VK_FORMAT_R16G16B16A16_SFLOAT;

    // 8-bit unsigned integer, 1 channel, linear
    static constexpr VkFormat SingleChannelUnorm8 = VK_FORMAT_R8_UNORM;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

#ifndef BLIB_RENDER_VULKAN_TEXTUREFORMAT_HPP
#define BLIB_RENDER_VULKAN_TEXTUREFORMAT_HPP

#include <BLIB/Vulkan.hpp>
#include <type_traits>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Common texture formats
 *
 * @ingroup Renderer
 */
struct TextureFormat {
    /// 8-bit unsigned integer, 4 channels, linear
    static constexpr VkFormat LinearRGBA32Bit = VK_FORMAT_R8G8B8A8_UNORM;

    /// 8-bit unsigned integer, 4 channels, sRGB
    static constexpr VkFormat SRGBA32Bit = VK_FORMAT_R8G8B8A8_SRGB;

    /// 16 bit signed float, linear
    static constexpr VkFormat HDRColor = VK_FORMAT_R16G16B16A16_SFLOAT;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
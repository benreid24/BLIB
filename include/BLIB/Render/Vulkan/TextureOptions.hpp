#ifndef BLIB_RENDER_VULKAN_TEXTUREOPTIONS_HPP
#define BLIB_RENDER_VULKAN_TEXTUREOPTIONS_HPP

#include <BLIB/Render/Vulkan/CommonTextureFormats.hpp>
#include <BLIB/Render/Vulkan/Sampler.hpp>
#include <BLIB/Render/Vulkan/SamplerOptions.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Basic POD struct containing texture creation parameters and defaults
 *
 * @ingroup Renderer
 */
struct TextureOptions {
    VkFormat format              = CommonTextureFormats::LinearRGBA32Bit;
    SamplerOptions::Type sampler = SamplerOptions::Type::FilteredBorderClamped;
    bool genMipmaps              = false;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

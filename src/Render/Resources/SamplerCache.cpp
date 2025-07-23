#include <BLIB/Render/Resources/SamplerCache.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <stdexcept>

namespace bl
{
namespace rc
{
namespace res
{
SamplerCache::SamplerCache(vk::VulkanState& vs)
: vulkanState(vs)
, createTable{}
, samplerTable{} {}

void SamplerCache::init() {
    using T = vk::SamplerOptions::Type;

    // common settings
    VkSamplerCreateInfo common{};
    common.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    common.anisotropyEnable = VK_TRUE;
    common.maxAnisotropy    = vulkanState.physicalDeviceProperties.limits.maxSamplerAnisotropy;
    common.addressModeU     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    common.addressModeV     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    common.addressModeW     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    common.unnormalizedCoordinates = VK_FALSE;
    common.borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    common.compareEnable           = VK_FALSE;
    common.compareOp               = VK_COMPARE_OP_ALWAYS;
    common.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    common.mipLodBias              = 0.0f;
    common.minLod                  = 0.0f;
    common.maxLod                  = VK_LOD_CLAMP_NONE;

    // no filter sampler
    auto& noFilterBorderClamp     = createTable[T::NoFilterBorderClamped];
    noFilterBorderClamp           = common;
    noFilterBorderClamp.minFilter = VK_FILTER_NEAREST;
    noFilterBorderClamp.magFilter = VK_FILTER_NEAREST;

    // no filter edge clamped
    auto& noFilterEdgeClamp        = createTable[T::NoFilterEdgeClamped];
    noFilterEdgeClamp              = common;
    noFilterEdgeClamp.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    noFilterEdgeClamp.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    noFilterEdgeClamp.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    noFilterEdgeClamp.minFilter    = VK_FILTER_NEAREST;
    noFilterEdgeClamp.magFilter    = VK_FILTER_NEAREST;

    // all filter edge clamped
    auto& filterEdgeClamp        = createTable[T::FilteredEdgeClamped];
    filterEdgeClamp              = common;
    filterEdgeClamp.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    filterEdgeClamp.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    filterEdgeClamp.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    filterEdgeClamp.minFilter    = VK_FILTER_LINEAR;
    filterEdgeClamp.magFilter    = VK_FILTER_LINEAR;

    // min filter sampler
    auto& minFilterBorderClamp        = createTable[T::MinFilterBorderClamped];
    minFilterBorderClamp              = common;
    minFilterBorderClamp.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    minFilterBorderClamp.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    minFilterBorderClamp.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    minFilterBorderClamp.minFilter    = VK_FILTER_LINEAR;
    minFilterBorderClamp.magFilter    = VK_FILTER_NEAREST;

    // mag filter sampler
    auto& magFilterBorderClamp     = createTable[T::MagFilterBorderClamped];
    magFilterBorderClamp           = minFilterBorderClamp;
    magFilterBorderClamp.minFilter = VK_FILTER_NEAREST;
    magFilterBorderClamp.magFilter = VK_FILTER_LINEAR;

    // all filter sampler
    auto& filteredBorderClamp     = createTable[T::FilteredBorderClamped];
    filteredBorderClamp           = magFilterBorderClamp;
    filteredBorderClamp.minFilter = VK_FILTER_LINEAR;
    filteredBorderClamp.magFilter = VK_FILTER_LINEAR;

    // filtered tiled sampler
    auto& filterRepeat        = createTable[T::FilteredRepeated];
    filterRepeat              = filteredBorderClamp;
    filterRepeat.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    filterRepeat.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    filterRepeat.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    // shadow map sampler
    auto& shadow            = createTable[T::ShadowMap];
    shadow                  = common;
    shadow.addressModeU     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    shadow.addressModeV     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    shadow.addressModeW     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    shadow.minFilter        = VK_FILTER_LINEAR;
    shadow.magFilter        = VK_FILTER_LINEAR;
    shadow.anisotropyEnable = VK_FALSE;
    shadow.borderColor      = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    shadow.compareEnable    = VK_TRUE;
    shadow.compareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
    shadow.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
}

void SamplerCache::cleanup() {
    for (unsigned int t = 0; t < vk::SamplerOptions::TypeCount; ++t) {
        if (samplerTable[t]) { vkDestroySampler(vulkanState.device, samplerTable[t], nullptr); }
    }
}

vk::Sampler SamplerCache::getSampler(const vk::SamplerOptions& options) {
    // TODO - add other params to options
    VkSampler& sampler = samplerTable[options.type];
    if (!sampler) {
        if (VK_SUCCESS !=
            vkCreateSampler(vulkanState.device, &createTable[options.type], nullptr, &sampler)) {
            throw std::runtime_error("Failed to create sampler");
        }
    }
    return vk::Sampler(vulkanState, sampler, false);
}

} // namespace res
} // namespace rc
} // namespace bl

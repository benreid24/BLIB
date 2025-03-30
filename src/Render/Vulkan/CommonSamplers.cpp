#include <BLIB/Render/Vulkan/CommonSamplers.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <stdexcept>

namespace bl
{
namespace rc
{
namespace vk
{
CommonSamplers::CommonSamplers(VulkanState& vs)
: vulkanState(vs) {}

void CommonSamplers::init() {
    // common settings
    VkSamplerCreateInfo create{};
    create.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create.anisotropyEnable = VK_TRUE;
    create.maxAnisotropy    = vulkanState.physicalDeviceProperties.limits.maxSamplerAnisotropy;
    create.addressModeU     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    create.addressModeV     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    create.addressModeW     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    create.unnormalizedCoordinates = VK_FALSE;
    create.borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

    // unused for now
    create.compareEnable = VK_FALSE;
    create.compareOp     = VK_COMPARE_OP_ALWAYS;
    create.mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    create.mipLodBias    = 0.0f;
    create.minLod        = 0.0f;
    create.maxLod        = 0.0f;

    // no filter sampler
    create.minFilter = VK_FILTER_NEAREST;
    create.magFilter = VK_FILTER_NEAREST;
    if (VK_SUCCESS != vkCreateSampler(vulkanState.device, &create, nullptr, &noFilterClamped)) {
        throw std::runtime_error("Failed to create sampler");
    }

    // no filter edge clamped
    create.maxAnisotropy = vulkanState.physicalDeviceProperties.limits.maxSamplerAnisotropy;
    create.addressModeU  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    create.addressModeV  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    create.addressModeW  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    if (VK_SUCCESS != vkCreateSampler(vulkanState.device, &create, nullptr, &noFilterEClamped)) {
        throw std::runtime_error("Failed to create sampler");
    }

    // min filter sampler
    create.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    create.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    create.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    create.minFilter    = VK_FILTER_LINEAR;
    create.magFilter    = VK_FILTER_NEAREST;
    if (VK_SUCCESS != vkCreateSampler(vulkanState.device, &create, nullptr, &minFilterClamped)) {
        throw std::runtime_error("Failed to create sampler");
    }

    // mag filter sampler
    create.minFilter = VK_FILTER_NEAREST;
    create.magFilter = VK_FILTER_LINEAR;
    if (VK_SUCCESS != vkCreateSampler(vulkanState.device, &create, nullptr, &magFilterClamped)) {
        throw std::runtime_error("Failed to create sampler");
    }

    // all filter sampler
    create.minFilter = VK_FILTER_LINEAR;
    create.magFilter = VK_FILTER_LINEAR;
    if (VK_SUCCESS != vkCreateSampler(vulkanState.device, &create, nullptr, &filteredClamped)) {
        throw std::runtime_error("Failed to create sampler");
    }

    // filtered tiled sampler
    create.minFilter    = VK_FILTER_LINEAR;
    create.magFilter    = VK_FILTER_LINEAR;
    create.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    if (VK_SUCCESS != vkCreateSampler(vulkanState.device, &create, nullptr, &filteredTiled)) {
        throw std::runtime_error("Failed to create sampler");
    }
}

void CommonSamplers::cleanup() {
    vkDestroySampler(vulkanState.device, noFilterClamped, nullptr);
    vkDestroySampler(vulkanState.device, noFilterEClamped, nullptr);
    vkDestroySampler(vulkanState.device, minFilterClamped, nullptr);
    vkDestroySampler(vulkanState.device, magFilterClamped, nullptr);
    vkDestroySampler(vulkanState.device, filteredClamped, nullptr);
    vkDestroySampler(vulkanState.device, filteredTiled, nullptr);
}

VkSampler CommonSamplers::getSampler(Sampler sampler) const {
    switch (sampler) {
    case Sampler::NoFilterBorderClamped:
        return noFilterClamped;

    case Sampler::NoFilterEdgeClamped:
        return noFilterEClamped;

    case Sampler::MinFilterBorderClamped:
        return minFilterClamped;

    case Sampler::MagFilterBorderClamped:
        return magFilterClamped;

    case Sampler::FilteredBorderClamped:
        return filteredClamped;

    case Sampler::FilteredRepeated:
        return filteredTiled;

    default:
        return noFilterClamped;
    }
}

} // namespace vk
} // namespace rc
} // namespace bl

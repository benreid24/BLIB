#ifndef BLIB_RENDER_VULKAN_COMMONSAMPLERS_HPP
#define BLIB_RENDER_VULKAN_COMMONSAMPLERS_HPP

#include <BLIB/Render/Vulkan/Sampler.hpp>
#include <BLIB/Render/Vulkan/SamplerOptions.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace vk
{
struct VulkanLayer;
}
namespace res
{

/**
 * @brief Helper class containing some common samplers to save on boilerplate
 *
 * @ingroup Renderer
 */
class SamplerCache {
public:
    /**
     * @brief Returns a sampler with no filtering and clamped to border. Max anisotropy.
     */
    VkSampler noFilterBorderClamped();

    /**
     * @brief Returns a sampler with no filtering and clamped to edge. Max anisotropy.
     */
    VkSampler noFilterEdgeClamped();

    /**
     * @brief Returns a sampler with min filtering and clamped to border. Max anisotropy.
     */
    VkSampler minFilterBorderClamped();

    /**
     * @brief Returns a sampler with mag filtering and clamped to border. Max anisotropy.
     */
    VkSampler magFilterBorderClamped();

    /**
     * @brief Returns a sampler with filtering and clamped to border. Max anisotropy.
     */
    VkSampler filteredBorderClamped();

    /**
     * @brief Returns a sampler with filtering and clamped to edge. Max anisotropy.
     */
    VkSampler filteredEdgeClamped();

    /**
     * @brief Returns a sampler with filtering and repeated addressing
     */
    VkSampler filteredRepeated();

    /**
     * @brief Returns the sampler to use for reading shadow maps
     */
    VkSampler shadowMap();

    /**
     * @brief Creates or fetches a sampler for the given options
     *
     * @param options The parameters the sampler must have
     * @return The sampler for the given enum value
     */
    vk::Sampler getSampler(const vk::SamplerOptions& options);

private:
    Renderer& renderer;
    VkSamplerCreateInfo createTable[vk::SamplerOptions::TypeCount];
    VkSampler samplerTable[vk::SamplerOptions::TypeCount];

    SamplerCache(Renderer& r);
    void init();
    void cleanup();

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkSampler SamplerCache::noFilterBorderClamped() {
    return getSampler({.type = vk::SamplerOptions::NoFilterBorderClamped});
}

inline VkSampler SamplerCache::noFilterEdgeClamped() {
    return getSampler({.type = vk::SamplerOptions::NoFilterEdgeClamped});
}

inline VkSampler SamplerCache::minFilterBorderClamped() {
    return getSampler({.type = vk::SamplerOptions::MinFilterBorderClamped});
}

inline VkSampler SamplerCache::magFilterBorderClamped() {
    return getSampler({.type = vk::SamplerOptions::MagFilterBorderClamped});
}

inline VkSampler SamplerCache::filteredBorderClamped() {
    return getSampler({.type = vk::SamplerOptions::FilteredBorderClamped});
}

inline VkSampler SamplerCache::filteredEdgeClamped() {
    return getSampler({.type = vk::SamplerOptions::FilteredEdgeClamped});
}

inline VkSampler SamplerCache::filteredRepeated() {
    return getSampler({.type = vk::SamplerOptions::FilteredRepeated});
}

inline VkSampler SamplerCache::shadowMap() {
    return getSampler({.type = vk::SamplerOptions::ShadowMap});
}

} // namespace res
} // namespace rc
} // namespace bl

#endif

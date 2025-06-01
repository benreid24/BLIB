#ifndef BLIB_RENDER_VULKAN_COMMONSAMPLERS_HPP
#define BLIB_RENDER_VULKAN_COMMONSAMPLERS_HPP

#include <BLIB/Render/Vulkan/Sampler.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Helper class containing some common samplers to save on boilerplate
 *
 * @ingroup Renderer
 */
class CommonSamplers {
public:
    /**
     * @brief Returns a sampler with no filtering and clamped to border. Max anisotropy.
     */
    VkSampler noFilterBorderClamped() const;

    /**
     * @brief Returns a sampler with no filtering and clamped to edge. Max anisotropy.
     */
    VkSampler noFilterEdgeClamped() const;

    /**
     * @brief Returns a sampler with min filtering and clamped to border. Max anisotropy.
     */
    VkSampler minFilterBorderClamped() const;

    /**
     * @brief Returns a sampler with mag filtering and clamped to border. Max anisotropy.
     */
    VkSampler magFilterBorderClamped() const;

    /**
     * @brief Returns a sampler with filtering and clamped to border. Max anisotropy.
     */
    VkSampler filteredBorderClamped() const;

    /**
     * @brief Returns a sampler with filtering and clamped to edge. Max anisotropy.
     */
    VkSampler filteredEdgeClamped() const;

    /**
     * @brief Returns a sampler with filtering and repeated addressing
     */
    VkSampler filteredRepeated() const;

    /**
     * @brief Returns the sampler to use for reading shadow maps
     */
    VkSampler shadowMap() const;

    /**
     * @brief Returns the sampler for the given enum value
     *
     * @param sampler The enum value to get the sampler for
     * @return The sampler for the given enum value
     */
    VkSampler getSampler(Sampler sampler) const;

private:
    VulkanState& vulkanState;
    VkSampler noFilterClamped;
    VkSampler noFilterEClamped;
    VkSampler minFilterClamped;
    VkSampler magFilterClamped;
    VkSampler filteredEClamped;
    VkSampler filteredClamped;
    VkSampler filteredTiled;
    VkSampler shadowMapSampler;

    CommonSamplers(VulkanState& vs);
    void init();
    void cleanup();

    friend struct VulkanState;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkSampler CommonSamplers::noFilterBorderClamped() const { return noFilterClamped; }

inline VkSampler CommonSamplers::noFilterEdgeClamped() const { return noFilterEClamped; }

inline VkSampler CommonSamplers::minFilterBorderClamped() const { return minFilterClamped; }

inline VkSampler CommonSamplers::magFilterBorderClamped() const { return magFilterClamped; }

inline VkSampler CommonSamplers::filteredBorderClamped() const { return filteredClamped; }

inline VkSampler CommonSamplers::filteredRepeated() const { return filteredTiled; }

inline VkSampler CommonSamplers::filteredEdgeClamped() const { return filteredEClamped; }

inline VkSampler CommonSamplers::shadowMap() const { return shadowMapSampler; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif

#ifndef BLIB_RENDER_VULKAN_COMMONSAMPLERS_HPP
#define BLIB_RENDER_VULKAN_COMMONSAMPLERS_HPP

#include <glad/vulkan.h>

namespace bl
{
namespace gfx
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
    constexpr VkSampler noFilterEdgeClamped() const;

    /**
     * @brief Returns a sampler with min filtering and clamped to border. Max anisotropy.
     */
    constexpr VkSampler minFilterEdgeClamped() const;

    /**
     * @brief Returns a sampler with mag filtering and clamped to border. Max anisotropy.
     */
    constexpr VkSampler magFilterEdgeClamped() const;

    /**
     * @brief Returns a sampler with filtering and clamped to border. Max anisotropy.
     */
    constexpr VkSampler filteredEdgeClamped() const;

    /**
     * @brief Returns a sampler with filtering and repeated addressing
     */
    constexpr VkSampler filteredRepeated() const;

private:
    VulkanState& vulkanState;
    VkSampler noFilterClamped;
    VkSampler minFilterClamped;
    VkSampler magFilterClamped;
    VkSampler filteredClamped;
    VkSampler filteredTiled;

    CommonSamplers(VulkanState& vs);
    void init();
    void cleanup();

    friend struct VulkanState;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkSampler CommonSamplers::noFilterEdgeClamped() const { return noFilterClamped; }

inline constexpr VkSampler CommonSamplers::minFilterEdgeClamped() const { return minFilterClamped; }

inline constexpr VkSampler CommonSamplers::magFilterEdgeClamped() const { return magFilterClamped; }

inline constexpr VkSampler CommonSamplers::filteredEdgeClamped() const { return filteredClamped; }

inline constexpr VkSampler CommonSamplers::filteredRepeated() const { return filteredTiled; }

} // namespace vk
} // namespace gfx
} // namespace bl

#endif

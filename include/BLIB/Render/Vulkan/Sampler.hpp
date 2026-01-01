#ifndef BLIB_RENDER_VULKAN_SAMPLER_HPP
#define BLIB_RENDER_VULKAN_SAMPLER_HPP

#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanLayer;

/**
 * @brief Thin wrapper for managed VkSampler objects
 *
 * @ingroup Renderer
 */
class Sampler {
public:
    /**
     * @brief Creates an empty sampler object
     */
    Sampler();

    /**
     * @brief Creates the sampler from the underlying Vulkan primitives
     *
     * @param device The Vulkan logical device
     * @param sampler The sampler handle
     * @param owner Whether this object owns the sampler. Pass false if it is shared
     */
    Sampler(VulkanLayer& vulkanState, VkSampler sampler, bool owner = true);

    /**
     * @brief Assumes ownership of the underlying sampler from the other object
     *
     * @param sampler The sampler handle to take from. Is invalidated if was owner
     */
    Sampler(Sampler&& sampler);

    /**
     * @brief Releases the held sampler if present and owned
     */
    ~Sampler();

    /**
     * @brief Assumes ownership of the underlying sampler from the other object
     *
     * @param sampler The sampler handle to take from. Is invalidated if was owner
     * @return A reference to this object
     */
    Sampler& operator=(Sampler&& sampler);

    /**
     * @brief Returns the underlying sampler
     */
    operator VkSampler() const { return sampler; }

    /**
     * @brief Returns the underlying sampler
     */
    VkSampler getSampler() const { return sampler; }

    /**
     * @brief Releases the underlying sampler if present and owned
     */
    void release();

    /**
     * @brief Defers the call to release for several frames to ensure the sampler is no longer used
     */
    void deferRelease();

private:
    VulkanLayer* vulkanState;
    VkSampler sampler;
    bool owner;

    Sampler(const Sampler&)            = delete;
    Sampler& operator=(const Sampler&) = delete;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

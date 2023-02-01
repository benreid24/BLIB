#ifndef BLIB_RENDER_VULKAN_COMMONDESCRIPTORSETLAYOUTS_HPP
#define BLIB_RENDER_VULKAN_COMMONDESCRIPTORSETLAYOUTS_HPP

#include <glad/vulkan.h>

namespace bl
{
namespace render
{
struct VulkanState;

/**
 * @brief Container struct for descriptor set layouts used across the renderer
 *
 * @ingroup Renderer
 */
struct CommonDescriptorSetLayouts {
    VkDescriptorSetLayout imageOverlay;

private:
    void init(VulkanState& vulkanState);
    void cleanup(VulkanState& vulkanState);

    friend struct VulkanState;
};

} // namespace render
} // namespace bl

#endif

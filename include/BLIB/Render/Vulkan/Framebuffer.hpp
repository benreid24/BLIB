#ifndef BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP
#define BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <BLIB/Vulkan.hpp>
#include <stdexcept>

namespace bl
{
namespace gfx
{
namespace vk
{
struct VulkanState;
class Swapchain;

/**
 * @brief Utility struct that wraps a Vulkan framebuffer.
 *
 * @ingroup Renderer
 */
class Framebuffer {
public:
    /**
     * @brief Construct an empty Framebuffer
     *
     */
    Framebuffer();

    /**
     * @brief Destroy the Framebuffer object
     *
     */
    ~Framebuffer();

    /**
     * @brief Creates (or recreates) the framebuffer
     *
     * @param vulkanState The renderer state
     * @param renderPass The render pass that will be used with the frame buffer
     * @param target The frame to render to
     */
    void create(VulkanState& vulkanState, VkRenderPass renderPass, const vk::AttachmentSet& target);

    /**
     * @brief Recreates the framebuffer if the underlying target has changed
     *
     * @param target The (potentially different) underlying target
     */
    void recreateIfChanged(const vk::AttachmentSet& target);

    /**
     * @brief Frees owned resources and invalidates this object
     *
     */
    void cleanup();

    /**
     * @brief Begins the render pass, recreating the framebuffer if necessary
     *
     * @param commandBuffer The primary command buffer to issue commands into
     * @param region The region of the framebuffer to render to
     * @param clearColors The colors to clear the framebuffer with on render pass begin
     * @param clearColorCount The number of clear colors
     * @param setViewportAndScissor True to set viewport/scissor to region, false to leave unset
     */
    void beginRender(VkCommandBuffer commandBuffer, const VkRect2D& region,
                     VkClearValue* clearColors, std::uint32_t clearColorCount,
                     bool setViewportAndScissor = true) const;

    /**
     * @brief Ends the render pass
     *
     * @param commandBuffer The primary command buffer to issue commands into
     */
    void finishRender(VkCommandBuffer commandBuffer) const;

private:
    VulkanState* vulkanState;
    VkRenderPass renderPass;
    const vk::AttachmentSet* target;
    VkFramebuffer framebuffer;
    VkImageView cachedAttachment;
};

} // namespace vk
} // namespace gfx
} // namespace bl

#endif

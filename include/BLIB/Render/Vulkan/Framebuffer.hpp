#ifndef BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP
#define BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <glad/vulkan.h>
#include <stdexcept>

namespace bl
{
namespace render
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
    void create(VulkanState& vulkanState, VkRenderPass renderPass, const AttachmentSet& target);

    /**
     * @brief Recreates the framebuffer if the underlying target has changed
     *
     * @param target The (potentially different) underlying target
     */
    void recreateIfChanged(const AttachmentSet& target);

    /**
     * @brief Frees owned resources and invalidates this object
     *
     */
    void cleanup();

    /**
     * @brief Begins the render pass and sets the viewport and scissor to sane defaults
     *
     * @param commandBuffer The primary command buffer to issue commands into
     * @param clearColors The colors to clear the framebuffer with on render pass begin
     * @param clearColorCount The number of clear colors
     */
    void beginRender(VkCommandBuffer commandBuffer, VkClearValue* clearColors,
                     std::uint32_t clearColorCount) const;

    /**
     * @brief Ends the render pass
     *
     * @param commandBuffer The primary command buffer to issue commands into
     */
    void finishRender(VkCommandBuffer commandBuffer) const;

private:
    VulkanState* vulkanState;
    VkRenderPass renderPass;
    const AttachmentSet* target;
    VkFramebuffer framebuffer;
    VkImageView cachedAttachment;
};

} // namespace render
} // namespace bl

#endif

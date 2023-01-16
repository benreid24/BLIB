#ifndef BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP
#define BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP

#include <glad/vulkan.h>
#include <stdexcept>
#include <BLIB/Render/Vulkan/RenderFrame.hpp>

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
     * @param vulkanState The renderer state
     */
    Framebuffer(VulkanState& vulkanState);

    /**
     * @brief Destroy the Framebuffer object
     *
     */
    ~Framebuffer();

    /**
     * @brief Creates (or recreates) the framebuffer
     *
     * @param renderPass The render pass that will be used with the frame buffer
     * @param target The frame to render to
     */
    void create(VkRenderPass renderPass, const RenderFrame& target);

    /**
     * @brief Frees owned resources and invalidates this object
     *
     */
    void cleanup();

    /**
     * @brief Begins the render pass and sets the viewport and scissor to sane defaults
     *
     * @param commandBuffer The primary command buffer to issue commands into
     */
    void beginRender(VkCommandBuffer commandBuffer);

    /**
     * @brief Ends the render pass
     *
     * @param commandBuffer The primary command buffer to issue commands into
     */
    void finishRender(VkCommandBuffer commandBuffer);

private:
    VulkanState& vulkanState;
    VkRenderPass renderPass;
    const RenderFrame* target;
    VkFramebuffer framebuffer;
};

} // namespace render
} // namespace bl

#endif

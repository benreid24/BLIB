#ifndef BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP
#define BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP

#include <glad/vulkan.h>
#include <stdexcept>

namespace bl
{
namespace render
{
struct VulkanState;

/**
 * @brief Utility struct that represents a swap frame in the window's swap chain. Manages
 *        synchronization primitives and resource cleanup. Also owns per-frame resources such as
 *        command pools and buffers to aid in thread safety for multi-threaded rendering
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
     * @param extent The size of the framebuffer render target area
     * @param attachments Pointer to the sequence of image views the framebuffer should wrap
     * @param attachmentCount The number of attachments in the sequence
     */
    void create(VkRenderPass renderPass, VkExtent2D extent, VkImageView* attachments,
                std::uint32_t attachmentCount);

    /**
     * @brief Frees owned resources and invalidates this object
     *
     */
    void cleanup();

    /**
     * @brief Blocks until prior render is complete. Creates new primary command buffer and begins
     *        the render pass init. Also sets the viewport and scissor to sane defaults
     *
     * @return VkCommandBuffer The primary command buffer to issue commands into
     */
    VkCommandBuffer beginRender();

    /**
     * @brief Ends the render pass and command buffer. Submits the generated render commands to the
     *        given queue to be executed. Must only be called with beginPrimaryRender()
     *
     * @param waitSemaphore Semaphore to block GPU with before beginning rendering
     * @param finishedSemaphore Semaphore to signal when rendering is completed
     * @param finishedFence Fence to signal when rendering is completed
     */
    void finishRender(VkSemaphore waitSemaphore, VkSemaphore finishedSemaphore,
                      VkFence finishedFence);

private:
    VulkanState& vulkanState;
    VkRenderPass renderPass;

    // owned
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkExtent2D extent;
    VkFramebuffer framebuffer;

    void beginCommon();
};

} // namespace render
} // namespace bl

#endif

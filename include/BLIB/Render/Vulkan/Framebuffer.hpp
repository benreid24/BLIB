#ifndef BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP
#define BLIB_RENDER_VULKAN_FRAMEBUFFER_HPP

#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <BLIB/Vulkan.hpp>
#include <stdexcept>

namespace bl
{
namespace rc
{
class Renderer;

namespace vk
{
class Swapchain;
class RenderPass;

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
     * @param renderer The renderer instance
     * @param renderPass The render pass that will be used with the frame buffer
     * @param target The frame to render to
     */
    void create(Renderer& renderer, const RenderPass* renderPass, const vk::AttachmentSet& target);

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
     * @brief Queues the framebuffer to be cleaned up later. Invalidates the object immediately
     */
    void deferCleanup();

    /**
     * @brief Begins the render pass, recreating the framebuffer if necessary. Scissor is set
     *
     * @param commandBuffer The primary command buffer to issue commands into
     * @param region The region of the framebuffer to render to
     * @param clearColors The colors to clear the framebuffer with on render pass begin
     * @param clearColorCount The number of clear colors
     * @param setViewport True to set viewport to region, false to leave unset
     * @param renderPass Optional render pass to use in place of the pass the fb was created with
     * @param clearOnRestart If true, clears the framebuffer on restart of the render pass
     */
    void beginRender(VkCommandBuffer commandBuffer, const VkRect2D& region,
                     const VkClearValue* clearColors, std::uint32_t clearColorCount,
                     bool setViewport = true, VkRenderPass renderPass = nullptr,
                     bool clearOnRestart = false);

    /**
     * @brief Ends the render pass
     *
     * @param commandBuffer The primary command buffer to issue commands into
     */
    void finishRender(VkCommandBuffer commandBuffer);

    /**
     * @brief Returns the attachment set of this framebuffer
     */
    const vk::AttachmentSet& getAttachmentSet() const { return *target; }

private:
    Renderer* renderer;
    const RenderPass* renderPass;
    const vk::AttachmentSet* target;
    VkFramebuffer framebuffer;
    VkImageView cachedAttachment;
    unsigned int renderStartCount;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

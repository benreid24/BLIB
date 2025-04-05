#ifndef BLIB_RENDER_GRAPH_ASSETS_FRAMEBUFFERASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_FRAMEBUFFERASSET_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/RenderPass.hpp>

namespace bl
{
namespace rc
{
class Observer;

namespace rgi
{
/**
 * @brief Intermediate base class for assets that provide framebuffers
 *
 * @ingroup Renderer
 */
class FramebufferAsset : public rg::Asset {
public:
    /**
     * @brief Destroys the asset
     */
    virtual ~FramebufferAsset() = default;

    /**
     * @brief Returns the current framebuffer to use. Defined in derived assets
     */
    virtual vk::Framebuffer& currentFramebuffer() = 0;

    /**
     * @brief Returns the render pass to use to render to the swap frame
     */
    const vk::RenderPass& getRenderPass() const { return *renderPass; }

    /**
     * @brief Intended to be called by observer when it resizes
     *
     * @param newSize The new size of the observer render region
     */
    void notifyResize(glm::u32vec2 newSize);

    /**
     * @brief Begins the render pass for this framebuffer asset
     *
     * @param commandBuffer The command buffer to write commands to
     * @param setViewport Whether to set the viewport for the render pass
     */
    void beginRender(VkCommandBuffer commandBuffer, bool setViewport);

    /**
     * @brief Ends the render pass for this framebuffer asset
     *
     * @param commandBuffer The command buffer to write commands to
     */
    void finishRender(VkCommandBuffer commandBuffer);

    /**
     * @brief Sets whether the framebuffer should clear its attachments if beginRender is called
     *        more than once
     *
     * @param shouldClear True to clear on restart, false to keep previous contents
     */
    void setShouldClearOnRestart(bool shouldClear);

    /**
     * @brief Returns the id of the render pass that renders this framebuffer
     */
    std::uint32_t getRenderPassId() const { return renderPassId; }

    /**
     * @brief Returns the viewport used when rendering
     */
    const VkViewport& getViewport() const { return viewport; }

protected:
    const std::uint32_t renderPassId;
    const VkViewport& viewport;
    const VkRect2D& scissor;
    const VkClearValue* clearColors;
    const std::uint32_t clearColorCount;
    bool clearOnRestart;
    const vk::RenderPass* renderPass;

    /**
     * @brief Creates the asset
     *
     * @param tag The asset tag
     * @param renderPassId The render pass the framebuffer uses
     * @param viewport The viewport of the framebuffer
     * @param scissor The scissor of the framebuffer
     * @param clearColors Pointer to array of clear colors for attachments
     * @param clearColorCount The number of clear colors
     * @param clearOnRestart Whether to clear attachments when restarting the render pass
     */
    FramebufferAsset(std::string_view tag, std::uint32_t renderPassId, const VkViewport& viewport,
                     const VkRect2D& scissor, const VkClearValue* clearColors,
                     const std::uint32_t clearColorCount);

    /**
     * @brief Called when the observer render region changes size
     *
     * @param newSize The new size of the observer render region
     */
    virtual void onResize(glm::u32vec2 newSize) = 0;

private:
    glm::u32vec2 cachedSize;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

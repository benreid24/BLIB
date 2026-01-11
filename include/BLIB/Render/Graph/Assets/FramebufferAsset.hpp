#ifndef BLIB_RENDER_GRAPH_ASSETS_FRAMEBUFFERASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_FRAMEBUFFERASSET_HPP

#include <BLIB/Render/Config/Limits.hpp>
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
     * @brief Returns the framebuffer at the given frame (or swap frame) index. Care should be taken
     *        to use valid indices for the type of assets being queried
     *
     * @param i The frame or swap frame index of the framebuffer to return
     * @return The framebuffer at the given index
     */
    virtual vk::Framebuffer& getFramebuffer(std::uint32_t i) = 0;

    /**
     * @brief Helper method to get per frame attachment sets. Should only be called if the
     *        underlying attachments are per frame
     */
    std::array<const vk::AttachmentSet*, cfg::Limits::MaxConcurrentFrames> getAttachmentSets();

    /**
     * @brief Returns the render pass to use to render to the swap frame
     */
    const vk::RenderPass& getRenderPass() const { return *renderPass; }

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

    /**
     * @brief Returns the scissor used when rendering
     */
    const VkRect2D& getScissor() const { return scissor; }

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
     * @param terminal Whether the asset is terminal
     * @param renderPassId The render pass the framebuffer uses
     * @param viewport The viewport of the framebuffer
     * @param scissor The scissor of the framebuffer
     * @param clearColors Pointer to array of clear colors for attachments
     * @param clearColorCount The number of clear colors
     * @param clearOnRestart Whether to clear attachments when restarting the render pass
     */
    FramebufferAsset(std::string_view tag, bool terminal, std::uint32_t renderPassId,
                     const VkViewport& viewport, const VkRect2D& scissor,
                     const VkClearValue* clearColors, const std::uint32_t clearColorCount);

private:
    unsigned int startCount;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

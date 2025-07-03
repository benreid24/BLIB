#ifndef BLIB_RENDER_GRAPH_ASSETS_FINALSWAPFRAMEASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_FINALSWAPFRAMEASSET_HPP

#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
class Swapchain;
}
namespace rgi
{
class DepthBuffer;

/**
 * @brief Asset for the swapchain image that an observer renders to
 *
 * @ingroup Renderer
 */
class FinalSwapframeAsset : public FramebufferAsset {
public:
    /**
     * @brief Creates a new asset
     *
     * @param framebuffers The swapchain image framebuffers
     * @param viewport The observer's viewport
     * @param scissor The observer's scissor
     * @param clearColors Pointer to array of clear colors for attachments
     * @param clearColorCount The number of clear colors
     */
    FinalSwapframeAsset(const VkViewport& viewport, const VkRect2D& scissor,
                        const VkClearValue* clearColors, const std::uint32_t clearColorCount);

    /**
     * @brief Does nothing
     */
    virtual ~FinalSwapframeAsset() = default;

    /**
     * @brief Returns the current framebuffer to use
     */
    virtual vk::Framebuffer& currentFramebuffer() override;

    /**
     * @brief Returns the framebuffer at the given swap frame index
     *
     * @param i The swap frame index of the framebuffer to return
     * @return The framebuffer at the given index
     */
    virtual vk::Framebuffer& getFramebuffer(std::uint32_t i) override;

private:
    vk::Swapchain* swapchain;
    DepthBuffer* depthBufferAsset;
    vk::PerSwapFrame<vk::StandardAttachmentSet> attachmentSets;
    vk::PerSwapFrame<vk::Framebuffer> framebuffers;

    virtual void doCreate(engine::Engine& engine, Renderer& renderer,
                          RenderTarget* observer) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doStartOutput(const rg::ExecutionContext& context) override;
    virtual void doEndOutput(const rg::ExecutionContext& context) override;
    virtual void onResize(glm::u32vec2 newSize) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

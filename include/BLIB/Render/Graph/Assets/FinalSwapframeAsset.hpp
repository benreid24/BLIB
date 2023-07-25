#ifndef BLIB_RENDER_GRAPH_ASSETS_FINALSWAPFRAMEASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_FINALSWAPFRAMEASSET_HPP

#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerSwapFrame.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
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
    FinalSwapframeAsset(vk::PerSwapFrame<vk::Framebuffer>& framebuffers, const VkViewport& viewport,
                        const VkRect2D& scissor, const VkClearValue* clearColors,
                        const std::uint32_t clearColorCount);

    /**
     * @brief Does nothing
     */
    virtual ~FinalSwapframeAsset() = default;

    /**
     * @brief Returns the current framebuffer to use
     */
    virtual vk::Framebuffer& currentFramebuffer() override;

private:
    vk::PerSwapFrame<vk::Framebuffer>& framebuffers;

    virtual void doCreate(engine::Engine& engine, Renderer& renderer, Observer* observer) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doPrepareForOutput(const rg::ExecutionContext& context) override;
    virtual void onResize(glm::u32vec2 newSize) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

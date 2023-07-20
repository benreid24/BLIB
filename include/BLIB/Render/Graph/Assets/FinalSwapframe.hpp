#ifndef BLIB_RENDER_GRAPH_ASSETS_FINALSWAPFRAME_HPP
#define BLIB_RENDER_GRAPH_ASSETS_FINALSWAPFRAME_HPP

#include <BLIB/Render/Graph/Asset.hpp>
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
class FinalSwapframe : public rg::Asset {
public:
    /**
     * @brief Creates a new asset
     *
     * @param framebuffers The swapchain image framebuffers
     * @param viewport The observer's viewport
     * @param scissor The observer's scissor
     */
    FinalSwapframe(vk::PerSwapFrame<vk::Framebuffer>& framebuffers, VkViewport& viewport,
                   VkRect2D& scissor);

    /**
     * @brief Does nothing
     */
    virtual ~FinalSwapframe() = default;

    vk::PerSwapFrame<vk::Framebuffer>& framebuffers;
    VkViewport& viewport;
    VkRect2D& scissor;

private:
    virtual void doCreate(engine::Engine& engine, Renderer& renderer) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doPrepareForOutput(const rg::ExecutionContext& context) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

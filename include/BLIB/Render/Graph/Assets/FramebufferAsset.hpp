#ifndef BLIB_RENDER_GRAPH_ASSETS_FRAMEBUFFERASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_FRAMEBUFFERASSET_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/RenderPass.hpp>

namespace bl
{
namespace rc
{
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
    constexpr const vk::RenderPass& getRenderPass() const { return *renderPass; }

    const std::uint32_t renderPassId;
    const VkViewport& viewport;
    const VkRect2D& scissor;
    const VkClearValue* clearColors;
    const std::uint32_t clearColorCount;

protected:
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
     */
    FramebufferAsset(std::string_view tag, std::uint32_t renderPassId, const VkViewport& viewport,
                     const VkRect2D& scissor, const VkClearValue* clearColors,
                     const std::uint32_t clearColorCount);
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

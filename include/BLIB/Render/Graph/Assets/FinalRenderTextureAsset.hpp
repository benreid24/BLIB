#ifndef BLIB_RENDER_GRAPH_ASSETS_FINALRENDERTEXTUREASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_FINALRENDERTEXTUREASSET_HPP

#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Asset for the framebuffer that a render texture renders to
 *
 * @ingroup Renderer
 */
class FinalRenderTextureAsset : public FramebufferAsset {
public:
    /**
     * @brief Creates a new asset
     *
     * @param framebuffers The render texture framebuffers
     * @param viewport The observer's viewport
     * @param scissor The observer's scissor
     * @param clearColors Pointer to array of clear colors for attachments
     * @param clearColorCount The number of clear colors
     */
    FinalRenderTextureAsset(vk::PerFrame<vk::Framebuffer>& framebuffers, const VkViewport& viewport,
                            const VkRect2D& scissor, const VkClearValue* clearColors,
                            const std::uint32_t clearColorCount);

    /**
     * @brief Does nothing
     */
    virtual ~FinalRenderTextureAsset() = default;

    /**
     * @brief Returns the current framebuffer to use
     */
    virtual vk::Framebuffer& currentFramebuffer() override;

private:
    vk::PerFrame<vk::Framebuffer>& framebuffers;

    virtual void doCreate(engine::Engine& engine, Renderer& renderer,
                          RenderTarget* observer) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doPrepareForOutput(const rg::ExecutionContext& context) override;
    virtual void onResize(glm::u32vec2 newSize) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

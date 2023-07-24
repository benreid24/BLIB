#ifndef BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP

#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Asset for a standard render target consisting of a single color and depth attachment
 *
 * @ingroup Renderer
 */
class StandardTargetAsset : public FramebufferAsset {
public:
    /**
     * @brief Creates a new asset but does not allocate the attachments
     *
     * @param renderPassId The render pass to use while rendering
     * @param viewport The viewport to use
     * @param scissor The scissor to use
     * @param clearColors The clear colors to start the render pass with
     * @param clearColorCount The number of clear colors
     */
    StandardTargetAsset(std::uint32_t renderPassId, const VkViewport& viewport,
                        const VkRect2D& scissor, const VkClearValue* clearColors,
                        const std::uint32_t clearColorCount);

    virtual ~StandardTargetAsset() = default;

    /**
     * @brief Returns the current framebuffer to use
     */
    virtual vk::Framebuffer& currentFramebuffer() override;

private:
    Renderer* renderer;
    vk::PerFrame<vk::StandardAttachmentBuffers> images;
    vk::PerFrame<vk::Framebuffer> framebuffers;

    virtual void doCreate(engine::Engine& engine, Renderer& renderer) override;
    virtual void doPrepareForInput(const rg::ExecutionContext& context) override;
    virtual void doPrepareForOutput(const rg::ExecutionContext& context) override;
    virtual void onResize(glm::u32vec2 newSize) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

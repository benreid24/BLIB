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
     */
    StandardTargetAsset();

    /**
     * @brief Frees resources
     */
    virtual ~StandardTargetAsset() = default;

    /**
     * @brief Returns the current framebuffer to use
     */
    virtual vk::Framebuffer& currentFramebuffer() override;

    /**
     * @brief Returns the framebuffer at the given frame index
     *
     * @param i The frame index of the framebuffer to return
     * @return The framebuffer at the given index
     */
    virtual vk::Framebuffer& getFramebuffer(std::uint32_t i) override;

    /**
     * @brief Returns the images that are rendered to
     */
    constexpr vk::PerFrame<vk::StandardAttachmentBuffers>& getImages() { return images; }

private:
    Renderer* renderer;
    RenderTarget* observer;
    vk::PerFrame<vk::StandardAttachmentBuffers> images;
    vk::PerFrame<vk::Framebuffer> framebuffers;
    VkViewport cachedViewport;
    VkRect2D cachedScissor;

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

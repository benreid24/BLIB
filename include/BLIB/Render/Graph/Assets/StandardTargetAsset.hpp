#ifndef BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Assets/RenderPassBehavior.hpp>
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
 * @tparam RenderPassId The id of the render pass to use
 * @tparam ColorFormat The format of the color attachment
 * @tparam RenderPassMode Whether this asset is responsible for render pass start/stop
 * @ingroup Renderer
 */
template<std::uint32_t RenderPassId, VkFormat ColorFormat,
         RenderPassBehavior RenderPassMode = RenderPassBehavior::StartedByAsset>
class StandardTargetAsset : public FramebufferAsset {
public:
    /**
     * @brief Creates a new asset but does not allocate the attachments
     *
     * @param tag The tag the asset is being created for
     */
    StandardTargetAsset(std::string_view tag)
    : FramebufferAsset(tag, RenderPassId, cachedViewport, cachedScissor, nullptr, 2)
    , renderer(nullptr)
    , cachedViewport{}
    , cachedScissor{} {
        cachedViewport.minDepth = 0.f;
        cachedViewport.maxDepth = 1.f;
        cachedScissor.offset.x  = 0;
        cachedScissor.offset.y  = 0;
        cachedViewport.x        = 0.f;
        cachedViewport.y        = 0.f;
    }

    /**
     * @brief Frees resources
     */
    virtual ~StandardTargetAsset() = default;

    /**
     * @brief Returns the current framebuffer to use
     */
    virtual vk::Framebuffer& currentFramebuffer() override { return framebuffers.current(); }

    /**
     * @brief Returns the framebuffer at the given frame index
     *
     * @param i The frame index of the framebuffer to return
     * @return The framebuffer at the given index
     */
    virtual vk::Framebuffer& getFramebuffer(std::uint32_t i) override {
        return framebuffers.getRaw(i);
    }

    /**
     * @brief Returns the images that are rendered to
     */
    vk::PerFrame<vk::StandardAttachmentBuffers>& getImages() { return images; }

private:
    Renderer* renderer;
    RenderTarget* observer;
    vk::PerFrame<vk::StandardAttachmentBuffers> images;
    vk::PerFrame<vk::Framebuffer> framebuffers;
    VkViewport cachedViewport;
    VkRect2D cachedScissor;

    virtual void doCreate(engine::Engine&, Renderer& r, RenderTarget* o) override {
        renderer = &r;
        observer = o;
        images.emptyInit(r.vulkanState());
        framebuffers.emptyInit(r.vulkanState());
        renderPass = &renderer->renderPassCache().getRenderPass(renderPassId);
        onResize(o->getRegionSize());
    }

    virtual void doPrepareForInput(const rg::ExecutionContext&) override {}

    virtual void doStartOutput(const rg::ExecutionContext& context) override {
        if constexpr (RenderPassMode == RenderPassBehavior::StartedByAsset) {
            beginRender(context.commandBuffer, true);
        }
    }

    virtual void doEndOutput(const rg::ExecutionContext& context) override {
        if constexpr (RenderPassMode == RenderPassBehavior::StartedByAsset) {
            finishRender(context.commandBuffer);
        }
    }

    virtual void onResize(glm::u32vec2 newSize) override {
        clearColors                 = observer->getClearColors();
        cachedScissor.extent.width  = newSize.x;
        cachedScissor.extent.height = newSize.y;
        cachedViewport.width        = static_cast<float>(newSize.x);
        cachedViewport.height       = static_cast<float>(newSize.y);

        if (renderer) {
            images.init(
                renderer->vulkanState(), [this, newSize](vk::StandardAttachmentBuffers& image) {
                    image.create(renderer->vulkanState(), {newSize.x, newSize.y}, ColorFormat);
                });
            unsigned int i = 0;
            framebuffers.init(renderer->vulkanState(), [this, &i](vk::Framebuffer& fb) {
                fb.create(renderer->vulkanState(),
                          renderPass->rawPass(),
                          images.getRaw(i).attachmentSet());
                ++i;
            });
        }
    }
};

/**
 * @brief Helper typedef for standard target assets using regular color formats
 *
 * @ingroup Renderer
 */
using LDRStandardTargetAsset =
    StandardTargetAsset<cfg::RenderPassIds::StandardAttachmentPass, vk::TextureFormat::SRGBA32Bit>;

/**
 * @brief Helper typedef for standard target assets using HDR color formats
 *
 * @ingroup Renderer
 */
using HDRStandardTargetAsset =
    StandardTargetAsset<cfg::RenderPassIds::HDRAttachmentPass, vk::TextureFormat::HDRColor>;

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

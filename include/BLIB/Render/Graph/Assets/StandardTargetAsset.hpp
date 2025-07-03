#ifndef BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_STANDARDATTACHMENTASSET_HPP

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Assets/RenderPassBehavior.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>

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

        addDependency(rg::AssetTags::DepthBuffer);
    }

    /**
     * @brief Frees resources
     */
    virtual ~StandardTargetAsset() = default;

    /**
     * @brief Returns the current framebuffer to use
     */
    virtual vk::Framebuffer& currentFramebuffer() override { return framebuffer; }

    /**
     * @brief Returns the framebuffer at the given frame index
     *
     * @param i The frame index of the framebuffer to return
     * @return The framebuffer at the given index
     */
    virtual vk::Framebuffer& getFramebuffer(std::uint32_t) override { return framebuffer; }

private:
    Renderer* renderer;
    RenderTarget* observer;
    vk::Image colorImage;
    DepthBuffer* depthBufferAsset;
    vk::StandardAttachmentSet attachmentSet;
    vk::Framebuffer framebuffer;
    VkViewport cachedViewport;
    VkRect2D cachedScissor;

    virtual void doCreate(engine::Engine&, Renderer& r, RenderTarget* o) override {
        renderer   = &r;
        observer   = o;
        renderPass = &renderer->renderPassCache().getRenderPass(renderPassId);

        depthBufferAsset = dynamic_cast<DepthBuffer*>(getDependency(0));
        if (!depthBufferAsset) {
            throw std::runtime_error("FinalSwapframeAsset requires a DepthBuffer dependency");
        }

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
        attachmentSet.setRenderExtent({newSize.x, newSize.y});
        clearColors                 = observer->getClearColors();
        cachedScissor.extent.width  = newSize.x;
        cachedScissor.extent.height = newSize.y;
        cachedViewport.width        = static_cast<float>(newSize.x);
        cachedViewport.height       = static_cast<float>(newSize.y);

        if (renderer) {
            depthBufferAsset->onResize(newSize);
            colorImage.create(renderer->vulkanState(),
                              vk::Image::Type::Image2D,
                              ColorFormat,
                              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                              {newSize.x, newSize.y},
                              VK_IMAGE_ASPECT_COLOR_BIT,
                              VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
            attachmentSet.setAttachments(colorImage.getImage(),
                                         colorImage.getView(),
                                         depthBufferAsset->getBuffer().getImage(),
                                         depthBufferAsset->getBuffer().getView());
            framebuffer.create(renderer->vulkanState(), renderPass->rawPass(), attachmentSet);
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

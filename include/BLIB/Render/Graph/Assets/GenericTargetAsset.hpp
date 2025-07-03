#ifndef BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/DepthAttachmentType.hpp>
#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Assets/RenderPassBehavior.hpp>
#include <BLIB/Render/Graph/Assets/TargetSize.hpp>
#include <BLIB/Render/Vulkan/AttachmentBufferSet.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/GenericAttachmentSet.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <utility>

// if getting circular include errors start here
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Generic asset for a templated set of attachments
 *
 * @tparam RenderPassId The id of the render pass to use
 * @tparam AttachmentCount The number of attachments to create
 * @tparam RenderPassMode Whether this asset is responsible for render pass start/stop
 * @ingroup Renderer
 */
template<std::uint32_t RenderPassId, std::uint32_t AttachmentCount,
         RenderPassBehavior RenderPassMode, DepthAttachmentType DepthAttachment>
class GenericTargetAsset : public FramebufferAsset {
public:
    static constexpr std::uint32_t TotalAttachmentCount =
        AttachmentCount + (DepthAttachment != DepthAttachmentType::None ? 1 : 0);

private:
    template<std::size_t... Is>
    static std::array<VkImageAspectFlags, TotalAttachmentCount> makeAspectArray(
        const std::array<VkFormat, AttachmentCount>& imageFormats,
        const std::array<VkImageUsageFlags, AttachmentCount>& imageUsages,
        std::index_sequence<Is...>) {
        return {vk::VulkanState::guessImageAspect(
            Is < AttachmentCount ? imageFormats[Is] : VK_FORMAT_D24_UNORM_S8_UINT,
            Is < AttachmentCount ? imageUsages[Is] :
                                   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)...};
    }

public:
    /**
     * @brief Creates a new asset but does not allocate the attachments
     *
     * @param tag The tag the asset is being created for
     * @param imageFormats The formats of the attachments
     * @param imageUsages How the attachments will be used
     * @param clearColors Pointer to array of clear colors for attachments
     * @param size The sizing behavior of the target
     */
    GenericTargetAsset(std::string_view tag,
                       const std::array<VkFormat, AttachmentCount>& imageFormats,
                       const std::array<VkImageUsageFlags, AttachmentCount>& imageUsages,
                       const std::array<VkClearValue, AttachmentCount>& clearColors,
                       const TargetSize& size)
    : FramebufferAsset(tag, RenderPassId, cachedViewport, cachedScissor, clearColors.data(),
                       AttachmentCount)
    , size(size)
    , renderer(nullptr)
    , attachmentFormats(imageFormats)
    , attachmentUsages(imageUsages)
    , attachmentSet(makeAspectArray(imageFormats, imageUsages,
                                    std::make_index_sequence<TotalAttachmentCount>{}),
                    1)
    , cachedViewport{}
    , cachedScissor{} {
        cachedViewport.minDepth = 0.f;
        cachedViewport.maxDepth = 1.f;
        cachedScissor.offset.x  = 0;
        cachedScissor.offset.y  = 0;
        cachedViewport.x        = 0.f;
        cachedViewport.y        = 0.f;

        if constexpr (DepthAttachment == DepthAttachmentType::SharedDepthBuffer) {
            addDependency(rg::AssetTags::DepthBuffer);
        }
    }

    /**
     * @brief Frees resources
     */
    virtual ~GenericTargetAsset() = default;

    /**
     * @brief Returns the current framebuffer to use
     */
    virtual vk::Framebuffer& currentFramebuffer() override { return framebuffer; }

    /**
     * @brief Returns the framebuffer at the given frame index
     *
     * @param Unused
     * @return The framebuffer at the given index
     */
    virtual vk::Framebuffer& getFramebuffer(std::uint32_t) override { return framebuffer; }

    /**
     * @brief Returns the images that are rendered to
     */
    vk::PerFrame<vk::AttachmentBufferSet<AttachmentCount>>& getImages() { return images; }

private:
    const TargetSize size;
    Renderer* renderer;
    RenderTarget* observer;
    const std::array<VkFormat, AttachmentCount>& attachmentFormats;
    const std::array<VkImageUsageFlags, AttachmentCount>& attachmentUsages;
    vk::AttachmentBufferSet<AttachmentCount> images;
    DepthBuffer* depthBufferAsset;
    vk::GenericAttachmentSet<TotalAttachmentCount> attachmentSet;
    vk::Framebuffer framebuffer;
    VkViewport cachedViewport;
    VkRect2D cachedScissor;

    virtual void doCreate(engine::Engine&, Renderer& r, RenderTarget* o) override {
        renderer   = &r;
        observer   = o;
        renderPass = &renderer->renderPassCache().getRenderPass(renderPassId);
        if constexpr (DepthAttachment == DepthAttachmentType::SharedDepthBuffer) {
            depthBufferAsset = dynamic_cast<DepthBuffer*>(getDependency(0));
            if (!depthBufferAsset) {
                throw std::runtime_error("GenericTargetAsset requires a DepthBuffer dependency");
            }
        }
        else { depthBufferAsset = nullptr; }
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
        switch (size.type) {
        case TargetSize::FixedSize:
            // bail if our size already matches
            if (cachedScissor.extent.width == size.size.x &&
                cachedScissor.extent.height == size.size.y) {
                return;
            }
            cachedScissor.extent.width  = size.size.x;
            cachedScissor.extent.height = size.size.y;
            cachedViewport.width        = static_cast<float>(size.size.x);
            cachedViewport.height       = static_cast<float>(size.size.y);
            break;

        case TargetSize::ObserverSize:
            cachedScissor.extent.width  = newSize.x;
            cachedScissor.extent.height = newSize.y;
            cachedViewport.width        = static_cast<float>(newSize.x);
            cachedViewport.height       = static_cast<float>(newSize.y);
            break;

        case TargetSize::ObserverSizeRatio:
            cachedViewport.width        = static_cast<float>(newSize.x) * size.ratio.x;
            cachedViewport.height       = static_cast<float>(newSize.y) * size.ratio.y;
            cachedScissor.extent.width  = static_cast<std::uint32_t>(cachedViewport.width);
            cachedScissor.extent.height = static_cast<std::uint32_t>(cachedViewport.height);
            break;
        }

        if (renderer) {
            images.create(renderer->vulkanState(),
                          {cachedScissor.extent.width, cachedScissor.extent.height},
                          attachmentFormats,
                          attachmentUsages);
            attachmentSet.setRenderExtent(cachedScissor.extent);
            attachmentSet.copy(images.attachmentSet(), AttachmentCount);
            if constexpr (DepthAttachment == DepthAttachmentType::SharedDepthBuffer) {
                depthBufferAsset->onResize(newSize);
                attachmentSet.setAttachment(AttachmentCount,
                                            depthBufferAsset->getBuffer().getImage(),
                                            depthBufferAsset->getBuffer().getView());
            }
            framebuffer.create(renderer->vulkanState(), renderPass->rawPass(), attachmentSet);
        }
    }
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

#ifndef BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP

#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Events/TextureFormatChanged.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/DepthAttachmentType.hpp>
#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Assets/MSAABehavior.hpp>
#include <BLIB/Render/Graph/Assets/RenderPassBehavior.hpp>
#include <BLIB/Render/Graph/Assets/TargetSize.hpp>
#include <BLIB/Render/Settings.hpp>
#include <BLIB/Render/Vulkan/AttachmentImageSet.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Signals/Listener.hpp>
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
 * @tparam AttachmentCount The number of attachments to create, excluding depth & resolve
 * @tparam RenderPassMode Whether this asset is responsible for render pass start/stop
 * @ingroup Renderer
 */
template<std::uint32_t RenderPassId, std::uint32_t AttachmentCount,
         RenderPassBehavior RenderPassMode, DepthAttachmentType DepthAttachment,
         MSAABehavior MSAA = MSAABehavior::Disabled>
class GenericTargetAsset
: public FramebufferAsset
, public sig::Listener<event::SettingsChanged, event::TextureFormatChanged> {
public:
    static constexpr bool UsesMSAA = MSAA & MSAABehavior::UseSettings;

    static constexpr std::uint32_t DepthAttachmentCount =
        DepthAttachment != DepthAttachmentType::None ? 1 : 0;

    static constexpr std::uint32_t ResolveAttachmentCount =
        (MSAA & MSAABehavior::ResolveAttachments) ? AttachmentCount : 0;

    static constexpr std::uint32_t RenderedAttachmentCount = AttachmentCount + DepthAttachmentCount;

    static constexpr std::uint32_t TotalAttachmentCount =
        AttachmentCount + DepthAttachmentCount + ResolveAttachmentCount;

public:
    /**
     * @brief Creates a new asset but does not allocate the attachments
     *
     * @param tag The tag the asset is being created for
     * @param terminal Whether the asset is terminal
     * @param imageFormats The formats of the attachments
     * @param imageUsages How the attachments will be used
     * @param clearColors Pointer to array of clear colors for attachments
     * @param size The sizing behavior of the target
     */
    GenericTargetAsset(std::string_view tag, bool terminal,
                       const std::array<vk::SemanticTextureFormat, AttachmentCount>& imageFormats,
                       const std::array<VkImageUsageFlags, AttachmentCount>& imageUsages,
                       const std::array<VkClearValue, RenderedAttachmentCount>& clearColors,
                       const TargetSize& size)
    : FramebufferAsset(tag, terminal, RenderPassId, cachedViewport, cachedScissor,
                       clearColors.data(), RenderedAttachmentCount)
    , size(size)
    , renderer(nullptr)
    , attachmentFormats(imageFormats)
    , attachmentUsages(imageUsages)
    , depthBufferAsset(nullptr)
    , depthBufferView(VK_NULL_HANDLE)
    , cachedViewport{}
    , cachedScissor{} {
        cachedViewport.minDepth = 0.f;
        cachedViewport.maxDepth = 1.f;
        cachedScissor.offset.x  = 0;
        cachedScissor.offset.y  = 0;
        cachedViewport.x        = 0.f;
        cachedViewport.y        = 0.f;

        for (std::uint32_t i = 0; i < AttachmentCount; ++i) {
            attachmentSet.setAttachmentAspect(i,
                                              vk::VulkanState::guessImageAspect(imageFormats[i]));
        }
        if constexpr (DepthAttachment == DepthAttachmentType::SharedDepthBuffer) {
            addDependency(rg::AssetTags::DepthBuffer);
            attachmentSet.setAttachmentAspect(
                AttachmentCount, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        }
        if constexpr (ResolveAttachmentCount > 0) {
            for (std::uint32_t i = 0; i < AttachmentCount; ++i) {
                attachmentSet.setAttachmentAspect(
                    i + AttachmentCount + DepthAttachmentCount,
                    vk::VulkanState::guessImageAspect(imageFormats[i]));
            }
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

private:
    const TargetSize size;
    Renderer* renderer;
    RenderTarget* observer;
    const std::array<vk::SemanticTextureFormat, AttachmentCount> attachmentFormats;
    const std::array<VkImageUsageFlags, AttachmentCount> attachmentUsages;
    vk::AttachmentImageSet images;
    vk::AttachmentImageSet resolveImages;
    DepthBuffer* depthBufferAsset;
    VkImageView depthBufferView;
    vk::AttachmentSet attachmentSet;
    vk::Framebuffer framebuffer;
    VkViewport cachedViewport;
    VkRect2D cachedScissor;

    virtual void doCreate(const rg::InitContext& ctx) override {
        renderer   = &ctx.renderer;
        observer   = &ctx.target;
        renderPass = &renderer->renderPassCache().getRenderPass(renderPassId);
        if constexpr (DepthAttachment == DepthAttachmentType::SharedDepthBuffer) {
            depthBufferAsset = dynamic_cast<DepthBuffer*>(getDependency(0));
            if (!depthBufferAsset) {
                throw std::runtime_error("GenericTargetAsset requires a DepthBuffer dependency");
            }
        }
        else { depthBufferAsset = nullptr; }
        onResize(ctx.target.getRegionSize());
        subscribe(ctx.renderer.getSignalChannel());
    }

    virtual void doPrepareForInput(const rg::ExecutionContext&) override {}

    virtual void doStartOutput(const rg::ExecutionContext& context) override {
        if constexpr (RenderPassMode == RenderPassBehavior::StartedByAsset) {
            if (depthBufferAsset && depthBufferAsset->getBuffer().getView() != depthBufferView) {
                depthBufferView = depthBufferAsset->getBuffer().getView();
                attachmentSet.setAttachment(AttachmentCount,
                                            depthBufferAsset->getBuffer().getImage(),
                                            depthBufferAsset->getBuffer().getView());
            }
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

        createAttachments();
    }

    void createAttachments() {
        if (renderer) {
            const bool createResolve =
                ResolveAttachmentCount > 0 &&
                renderer->getSettings().getAntiAliasing() != Settings::AntiAliasing::None;
            const VkSampleCountFlagBits sampleCount =
                UsesMSAA ? renderer->getSettings().getMSAASampleCount() : VK_SAMPLE_COUNT_1_BIT;

            images.create(renderer->vulkanState(),
                          AttachmentCount,
                          {cachedScissor.extent.width, cachedScissor.extent.height},
                          attachmentFormats.data(),
                          attachmentUsages.data(),
                          sampleCount);
            attachmentSet.setRenderExtent(cachedScissor.extent);

            if (createResolve) {
                resolveImages.create(renderer->vulkanState(),
                                     ResolveAttachmentCount,
                                     {cachedScissor.extent.width, cachedScissor.extent.height},
                                     attachmentFormats.data(),
                                     attachmentUsages.data(),
                                     VK_SAMPLE_COUNT_1_BIT);
                attachmentSet.setOutputIndex(ResolveAttachmentCount);
            }
            else if (UsesMSAA) {
                resolveImages.deferDestroy();
                attachmentSet.setOutputIndex(0);
            }

            std::uint32_t ac = AttachmentCount;
            attachmentSet.copy(images.attachmentSet(), AttachmentCount);
            if (createResolve) {
                attachmentSet.setAttachments(ac, resolveImages);
                ac += ResolveAttachmentCount;
            }
            if constexpr (DepthAttachment == DepthAttachmentType::SharedDepthBuffer) {
                depthBufferAsset->ensureValid(
                    {cachedScissor.extent.width, cachedScissor.extent.height}, sampleCount);
                attachmentSet.setAttachment(ac, depthBufferAsset->getBuffer());
                depthBufferView = depthBufferAsset->getBuffer().getView();
                ++ac;
            }
            attachmentSet.setAttachmentCount(ac);
            framebuffer.create(renderer->vulkanState(), renderPass, attachmentSet);
        }
    }

    virtual void process(const event::SettingsChanged& event) override {
        if (event.setting == event::SettingsChanged::Setting::AntiAliasing && UsesMSAA) {
            createAttachments();
        }
    }

    virtual void process(const event::TextureFormatChanged& event) override {
        bool needsUpdate = false;
        for (std::uint32_t i = 0; i < AttachmentCount; ++i) {
            if (attachmentFormats[i] == event.semanticFormat) {
                needsUpdate = true;
                break;
            }
        }
        if (depthBufferAsset && (event.semanticFormat == vk::SemanticTextureFormat::Depth ||
                                 event.semanticFormat == vk::SemanticTextureFormat::DepthStencil)) {
            needsUpdate = true;
        }
        if (needsUpdate) { createAttachments(); }
    }
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

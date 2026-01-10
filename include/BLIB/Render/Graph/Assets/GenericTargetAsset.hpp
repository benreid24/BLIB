#ifndef BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP

#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Events/TextureFormatChanged.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/DepthAttachmentType.hpp>
#include <BLIB/Render/Graph/Assets/DepthBuffer.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Assets/RenderPassBehavior.hpp>
#include <BLIB/Render/Settings.hpp>
#include <BLIB/Render/ShaderResources/AttachmentImageSetResource.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
#include <BLIB/Render/ShaderResources/StoreKey.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <string>
#include <utility>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Helper method to generate a shader resource key from an asset tag
 *
 * @tparam TShaderResource The type of shader resource
 * @param assetTag The tag of the asset using or owning the resource
 * @return A key to use to get the shader resource
 * @ingroup Renderer
 */
template<typename TShaderResource>
constexpr sr::Key<TShaderResource> makeShaderResourceKey(std::string_view assetTag) {
    return sr::Key<TShaderResource>(assetTag);
}

/**
 * @brief Helper method to generate a shader resource key from an asset tag
 *
 * @tparam TShaderResource The type of shader resource
 * @param assetTag The tag of the asset using or owning the resource
 * @param purpose The purpose of the asset
 * @param i The index of the asset in a multi-asset
 * @return A key to use to get the shader resource
 * @ingroup Renderer
 */
template<typename TShaderResource>
sr::Key<TShaderResource> makeShaderResourceKey(std::string_view assetTag, std::string_view purpose,
                                               unsigned int i) {
    std::string key = std::string(assetTag);
    if (!purpose.empty()) { key += "_" + std::string(purpose); }
    if (i > 0) { key += "_" + std::to_string(i); }
    return sr::Key<TShaderResource>({key.data(), key.size()});
}

/**
 * @brief Generic asset for a templated set of attachments
 *
 * @param TShaderResource The type of shader resource that provides the attachments
 * @param StoreKey The shader resource store to get the shader resource from
 * @tparam RenderPassId The id of the render pass to use
 * @tparam AttachmentCount The number of attachments to create, excluding depth & resolve
 * @tparam RenderPassMode Whether this asset is responsible for render pass start/stop
 * @ingroup Renderer
 */
template<typename TShaderResource, sr::StoreKey StoreKey, std::uint32_t RenderPassId,
         RenderPassBehavior RenderPassMode, DepthAttachmentType DepthAttachment>
class GenericTargetAsset
: public FramebufferAsset
, public sig::Listener<event::SettingsChanged, event::TextureFormatChanged> {
public:
    using Traits = sri::AttachmentImageSetResourceTraits<TShaderResource>;

    static constexpr bool UsesMSAA = Traits::MSAA & sri::MSAABehavior::UseSettings;

    static constexpr std::uint32_t DepthAttachmentCount =
        DepthAttachment != DepthAttachmentType::None ? 1 : 0;

    static constexpr std::uint32_t ResolveAttachmentCount =
        (Traits::MSAA & sri::MSAABehavior::ResolveAttachments) ? Traits::AttachmentCount : 0;

    static constexpr std::uint32_t RenderedAttachmentCount =
        Traits::AttachmentCount + DepthAttachmentCount;

    static constexpr std::uint32_t TotalAttachmentCount =
        Traits::AttachmentCount + DepthAttachmentCount + ResolveAttachmentCount;

public:
    /**
     * @brief Creates a new asset but does not allocate the attachments
     *
     * @param tag The tag the asset is being created for
     * @param terminal Whether the asset is terminal
     * @param clearColors Pointer to array of clear colors for attachments
     */
    GenericTargetAsset(std::string_view tag, bool terminal,
                       const std::array<VkClearValue, RenderedAttachmentCount>& clearColors)
    : FramebufferAsset(tag, terminal, RenderPassId, cachedViewport, cachedScissor,
                       clearColors.data(), RenderedAttachmentCount)
    , renderer(nullptr)
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

        for (std::uint32_t i = 0; i < Traits::AttachmentCount; ++i) {
            attachmentSet.setAttachmentAspect(
                i, vk::VulkanLayer::guessImageAspect(Traits::Formats[i]));
        }
        if constexpr (DepthAttachment == DepthAttachmentType::SharedDepthBuffer) {
            addDependency(rg::AssetTags::DepthBuffer);
            attachmentSet.setAttachmentAspect(
                Traits::AttachmentCount, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        }
        if constexpr (ResolveAttachmentCount > 0) {
            for (std::uint32_t i = 0; i < Traits::AttachmentCount; ++i) {
                attachmentSet.setAttachmentAspect(
                    i + Traits::AttachmentCount + DepthAttachmentCount,
                    vk::VulkanLayer::guessImageAspect(Traits::Formats[i]));
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
    Renderer* renderer;
    Traits::TResourceType* images;
    vk::AttachmentImageSet resolveImages;
    DepthBuffer* depthBufferAsset;
    VkImageView depthBufferView;
    vk::AttachmentSet attachmentSet;
    vk::Framebuffer framebuffer;
    VkViewport cachedViewport;
    VkRect2D cachedScissor;

    virtual void doCreate(const rg::InitContext& ctx) override {
        renderer   = &ctx.renderer;
        renderPass = &renderer->renderPassCache().getRenderPass(renderPassId);
        images     = ctx.getShaderResourceStore(StoreKey).getShaderResourceWithKey(
            makeShaderResourceKey<TShaderResource>(getTag(), getPurpose(), ctx.index));
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
                attachmentSet.setAttachment(Traits::AttachmentCount,
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
        switch (Traits::Size.type) {
        case sri::TargetSize::FixedSize:
            // bail if our size already matches
            if (cachedScissor.extent.width == Traits::Size.size.x &&
                cachedScissor.extent.height == Traits::Size.size.y) {
                return;
            }
            cachedScissor.extent.width  = Traits::Size.size.x;
            cachedScissor.extent.height = Traits::Size.size.y;
            cachedViewport.width        = static_cast<float>(Traits::Size.size.x);
            cachedViewport.height       = static_cast<float>(Traits::Size.size.y);
            break;

        case sri::TargetSize::ObserverSize:
            cachedScissor.extent.width  = newSize.x;
            cachedScissor.extent.height = newSize.y;
            cachedViewport.width        = static_cast<float>(newSize.x);
            cachedViewport.height       = static_cast<float>(newSize.y);
            break;

        case sri::TargetSize::ObserverSizeRatio:
            cachedViewport.width        = static_cast<float>(newSize.x) * Traits::Size.ratio.x;
            cachedViewport.height       = static_cast<float>(newSize.y) * Traits::Size.ratio.y;
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

            attachmentSet.setRenderExtent(cachedScissor.extent);

            if (createResolve) {
                resolveImages.create(*renderer,
                                     ResolveAttachmentCount,
                                     {cachedScissor.extent.width, cachedScissor.extent.height},
                                     Traits::Formats.data(),
                                     Traits::Usages.data(),
                                     VK_SAMPLE_COUNT_1_BIT);
                attachmentSet.setOutputIndex(ResolveAttachmentCount);
            }
            else if (UsesMSAA) {
                resolveImages.deferDestroy();
                attachmentSet.setOutputIndex(0);
            }

            std::uint32_t ac = Traits::AttachmentCount;
            attachmentSet.copy(images->getImages().attachmentSet(), Traits::AttachmentCount);
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
            framebuffer.create(*renderer, renderPass, attachmentSet);
        }
    }

    virtual void process(const event::SettingsChanged& event) override {
        if (event.setting == event::SettingsChanged::Setting::AntiAliasing && UsesMSAA) {
            createAttachments();
        }
    }

    virtual void process(const event::TextureFormatChanged& event) override {
        bool needsUpdate = false;
        for (std::uint32_t i = 0; i < Traits::AttachmentCount; ++i) {
            if (Traits::Formats[i] == event.semanticFormat) {
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

/**
 * @brief Trait deduction helper for GenericTargetAsset
 *
 * @tparam TGenericTargetAsset The generic target asset instance
 * @ingroup Renderer
 */
template<typename TGenericTargetAsset>
struct GenericTargetAssetTraits {
    static_assert(false, "GenericTargetAssetTraits not specialized for this type");
};

template<typename TShaderResource, sr::StoreKey StoreKey, std::uint32_t RenderPassId,
         RenderPassBehavior RenderPassMode, DepthAttachmentType DepthAttachment>
struct GenericTargetAssetTraits<
    GenericTargetAsset<TShaderResource, StoreKey, RenderPassId, RenderPassMode, DepthAttachment>> {
    using TAsset = GenericTargetAsset<TShaderResource, StoreKey, RenderPassId, RenderPassMode,
                                      DepthAttachment>;
    static constexpr std::uint32_t RenderedAttachmentCount = TAsset::RenderedAttachmentCount;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

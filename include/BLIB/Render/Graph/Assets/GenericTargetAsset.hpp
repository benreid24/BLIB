#ifndef BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
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
 * @tparam AttachmentCount The number of attachments to create, excluding depth & resolve
 * @tparam RenderPassMode Whether this asset is responsible for render pass start/stop
 * @ingroup Renderer
 */
template<std::uint32_t RenderPassId, std::uint32_t AttachmentCount,
         RenderPassBehavior RenderPassMode, DepthAttachmentType DepthAttachment,
         MSAABehavior MSAA = MSAABehavior::Disabled>
class GenericTargetAsset
: public FramebufferAsset
, public bl::event::Listener<event::SettingsChanged> {
public:
    static constexpr bool UsesMSAA = (MSAA & MSAABehavior::UseSettings) != 0;

    static constexpr std::uint32_t DepthAttachmentCount =
        DepthAttachment != DepthAttachmentType::None ? 1 : 0;

    static constexpr ResolveAttachmentCount =
        (MSAA & MSAABehavior::ResolveAttachments) != 0 ? AttachmentCount : 0;

    static constexpr RenderedAttachmentCount = AttachmentCount + DepthAttachmentCount;

    static constexpr std::uint32_t TotalAttachmentCount =
        AttachmentCount + DepthAttachmentCount + ResolveAttachmentCount;

private:
    static std::array<VkImageAspectFlags, TotalAttachmentCount> makeAspectArray(
        const std::array<VkFormat, AttachmentCount>& imageFormats,
        const std::array<VkImageUsageFlags, AttachmentCount>& imageUsages) {
        std::array<VkImageAspectFlags, TotalAttachmentCount> result{};

        unsigned int i = 0;
        while (i < AttachmentCount) {
            result[i] = vk::VulkanState::guessImageAspect(imageFormats[i], imageUsages[i]);
            ++i;
        }
        if constexpr (DepthAttachmentCount > 0) {
            result[i] = VK_IMAGE_ASPECT_DEPTH_BIT;
            ++i;
        }

        if constexpr (ResolveAttachmentCount > 0) {
            unsigned int j = 0;
            while (j < ResolveAttachmentCount) {
                result[i] = vk::VulkanState::guessImageAspect(imageFormats[i], imageUsages[i]);
                ++i;
                ++j;
            }
        }

        return result;
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
                       const std::array<VkClearValue, RenderedAttachmentCount>& clearColors,
                       const TargetSize& size)
    : FramebufferAsset(tag, RenderPassId, cachedViewport, cachedScissor, clearColors.data(),
                       RenderedAttachmentCount)
    , size(size)
    , renderer(nullptr)
    , attachmentFormats(imageFormats)
    , attachmentUsages(imageUsages)
    , depthBufferAsset(nullptr)
    , depthBufferView(VK_NULL_HANDLE)
    , attachmentSet(makeAspectArray(imageFormats, imageUsages), 1)
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

private:
    const TargetSize size;
    Renderer* renderer;
    RenderTarget* observer;
    const std::array<VkFormat, AttachmentCount> attachmentFormats;
    const std::array<VkImageUsageFlags, AttachmentCount> attachmentUsages;
    vk::AttachmentImageSet images;
    vk::AttachmentImageSet resolveImages;
    DepthBuffer* depthBufferAsset;
    VkImageView depthBufferView;
    vk::AttachmentSet attachmentSet;
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
        bl::event::Dispatcher::subscribe(this);
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
                          attachmentFormats,
                          attachmentUsages,
                          sampleCount);
            attachmentSet.setRenderExtent(cachedScissor.extent);

            if (createResolve) {
                resolveImages.create(renderer->vulkanState(),
                                     AttachmentCount,
                                     {cachedScissor.extent.width, cachedScissor.extent.height},
                                     attachmentFormats,
                                     attachmentUsages,
                                     sampleCount);
            }
            else if (UsesMSAA) { resolveImages.deferDestroy(); }

            std::uint32_t ac = AttachmentCount;
            attachmentSet.copy(images.attachmentSet(), AttachmentCount);
            if constexpr (DepthAttachment == DepthAttachmentType::SharedDepthBuffer) {
                depthBufferAsset->onResize(newSize);
                attachmentSet.setAttachment(AttachmentCount, depthBufferAsset->getBuffer());
                depthBufferView = depthBufferAsset->getBuffer().getView();
                ++ac;
            }
            if (createResolve) {
                attachmentSet.setAttachments(ac, resolveImages);
                ac += ResolveAttachmentCount;
            }
            attachmentSet.setAttachmentCount(ac);
            framebuffer.create(renderer->vulkanState(), renderPass->rawPass(), attachmentSet);
        }
    }

    virtual void observe(const event::SettingsChanged& event) override {
        if (event.setting == event::SettingsChanged::Setting::AntiAliasing && UsesMSAA) {
            createAttachments();
        }
    }
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

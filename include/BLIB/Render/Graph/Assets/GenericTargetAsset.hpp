#ifndef BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_GENERICTARGETASSET_HPP

#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Assets/TargetSize.hpp>
#include <BLIB/Render/Vulkan/AttachmentBufferSet.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

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
 * @tparam param AttachmentCount The number of attachments to create
 * @ingroup Renderer
 */
template<std::uint32_t RenderPassId, std::uint32_t AttachmentCount>
class GenericTargetAsset : public FramebufferAsset {
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
    virtual ~GenericTargetAsset() = default;

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
    vk::PerFrame<vk::AttachmentBufferSet<AttachmentCount>>& getImages() { return images; }

private:
    const TargetSize size;
    Renderer* renderer;
    RenderTarget* observer;
    const std::array<VkFormat, AttachmentCount>& attachmentFormats;
    const std::array<VkImageUsageFlags, AttachmentCount>& attachmentUsages;
    vk::PerFrame<vk::AttachmentBufferSet<AttachmentCount>> images;
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
    virtual void doPrepareForOutput(const rg::ExecutionContext&) override {}

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
            images.init(renderer->vulkanState(),
                        [this](vk::AttachmentBufferSet<AttachmentCount>& image) {
                            image.create(renderer->vulkanState(),
                                         {cachedScissor.extent.width, cachedScissor.extent.height},
                                         attachmentFormats,
                                         attachmentUsages);
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

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

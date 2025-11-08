#ifndef BLIB_RENDER_SHADERRESOURCES_ATTACHMENTIMAGESETRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_ATTACHMENTIMAGESETRESOURCE_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Events/TextureFormatChanged.hpp>
#include <BLIB/Render/ShaderResources/MSAABehavior.hpp>
#include <BLIB/Render/ShaderResources/ShaderResource.hpp>
#include <BLIB/Render/ShaderResources/TargetSize.hpp>
#include <BLIB/Render/Vulkan/AttachmentImageSet.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <array>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
/**
 * @brief Shader resource for a list of attachment images
 *
 * @ingroup Renderer
 */
template<unsigned int AttachmentCount,
         std::array<vk::SemanticTextureFormat, AttachmentCount> Formats,
         std::array<VkImageUsageFlags, AttachmentCount> Usages,
         TargetSize Size   = TargetSize(TargetSize::ObserverSize),
         MSAABehavior MSAA = MSAABehavior::Disabled>
class AttachmentImageSetResource
: public sr::ShaderResource
, public sig::Listener<event::SettingsChanged, event::TextureFormatChanged> {
    static constexpr bool FollowSettings = MSAA & MSAABehavior::UseSettings;

public:
    /**
     * @brief Creates the shader resource
     */
    AttachmentImageSetResource() = default;

    /**
     * @brief Destroys the shader resource
     */
    virtual ~AttachmentImageSetResource() = default;

    /**
     * @brief Provides access to the underlying images
     */
    vk::AttachmentImageSet& getImages() { return images; }

private:
    Renderer* renderer;
    RenderTarget* owner;
    vk::AttachmentImageSet images;
    unsigned int dirtyFrameCount;

    virtual void init(engine::Engine& engine, RenderTarget& o) override {
        renderer = &engine.renderer();
        owner    = &o;
        create();
        if constexpr (FollowSettings) { subscribe(renderer->getSignalChannel()); }
    }

    virtual void cleanup() override { images.deferDestroy(); }

    virtual void performTransfer() override { dirtyFrameCount = dirtyFrameCount >> 1; }

    virtual void copyFromSource() override {}

    virtual bool dynamicDescriptorUpdateRequired() const override { return dirtyFrameCount != 0; }

    virtual bool staticDescriptorUpdateRequired() const override { return dirtyFrameCount != 0; }

    virtual void process(const event::SettingsChanged& evt) override {
        if constexpr (FollowSettings) {
            if (evt.setting == event::SettingsChanged::AntiAliasing) { create(); }
        }
    }

    virtual void process(const event::TextureFormatChanged& evt) override {
        for (vk::SemanticTextureFormat fmt : Formats) {
            if (fmt == evt.semanticFormat) {
                create();
                return;
            }
        }
    }

    void onResize() {
        if (owner && Size.type != TargetSize::FixedSize) { create(); }
    }

    void create() {
        dirtyFrameCount = 0x1 << cfg::Limits::MaxConcurrentFrames;
        const VkSampleCountFlagBits sampleCount =
            FollowSettings ? renderer->getSettings().getMSAASampleCount() : VK_SAMPLE_COUNT_1_BIT;
        const glm::u32vec2 size = Size.getSize(owner->getRegionSize());
        images.create(renderer->vulkanState(),
                      AttachmentCount,
                      {size.x, size.y},
                      Formats.data(),
                      Usages.data(),
                      sampleCount);
    }
};

/**
 * @brief Template parameter deduction helper for attachment image sets
 *
 * @tparam T An instance of AttachmentImageSetResource
 * @ingroup Renderer
 */
template<typename T>
struct AttachmentImageSetResourceTraits {
    static_assert(false, "AttachmentImageSetResourceTraits not specialized for this type");
};

template<unsigned int AC, std::array<vk::SemanticTextureFormat, AC> Fmts,
         std::array<VkImageUsageFlags, AC> Usgs, TargetSize Sz, MSAABehavior AA>
struct AttachmentImageSetResourceTraits<AttachmentImageSetResource<AC, Fmts, Usgs, Sz, AA>> {
    static constexpr unsigned int AttachmentCount                      = AC;
    static constexpr std::array<vk::SemanticTextureFormat, AC> Formats = Fmts;
    static constexpr std::array<VkImageUsageFlags, AC> Usages          = Usgs;
    static constexpr TargetSize Size                                   = Sz;
    static constexpr MSAABehavior MSAA                                 = AA;
};

} // namespace sri
} // namespace rc
} // namespace bl

#endif

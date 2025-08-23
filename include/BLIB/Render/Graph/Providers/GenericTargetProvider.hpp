#ifndef BLIB_RENDER_GRAPH_PROVIDERS_GENERICTARGETPROVIDER_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_GENERICTARGETPROVIDER_HPP

#include <BLIB/Render/Graph/AssetProvider.hpp>
#include <BLIB/Render/Graph/Assets/GenericTargetAsset.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Provider for GenericTargetAsset assets
 *
 * @tparam RenderPassId The id of the render pass to use
 * @tparam AttachmentCount The number of attachments to create
 * @tparam RenderPassMode Whether this asset is responsible for render pass start/stop
 * @ingroup Renderer
 */
template<std::uint32_t RenderPassId, std::uint32_t AttachmentCount,
         RenderPassBehavior RenderPassMode, DepthAttachmentType DepthAttachment,
         MSAABehavior MSAA = MSAABehavior::Disabled>
class GenericTargetProvider : public rg::AssetProvider {
public:
    using TAsset =
        GenericTargetAsset<RenderPassId, AttachmentCount, RenderPassMode, DepthAttachment, MSAA>;
    static constexpr std::uint32_t RenderedAttachmentCount = TAsset::RenderedAttachmentCount;

    /**
     * @brief Creates the provider
     *
     * @param imageFormats The formats of the attachments
     * @param imageUsages How the attachments will be used
     */
    GenericTargetProvider(
        const TargetSize& size,
        const std::array<vk::SemanticTextureFormat, AttachmentCount>& imageFormats,
        const std::array<VkImageUsageFlags, AttachmentCount>& imageUsages,
        const std::array<VkClearValue, RenderedAttachmentCount>& clearColors)
    : size(size)
    , imageFormats(imageFormats)
    , imageUsages(imageUsages)
    , clearColors(clearColors) {}

    /**
     * @brief Creates a new generic target asset
     *
     * @param tag The tag to create the asset for
     * @return The newly created asset
     */
    virtual rg::Asset* create(std::string_view tag) override {
        return new TAsset(tag, imageFormats, imageUsages, clearColors, size);
    }

private:
    const TargetSize size;
    std::array<vk::SemanticTextureFormat, AttachmentCount> imageFormats;
    std::array<VkImageUsageFlags, AttachmentCount> imageUsages;
    std::array<VkClearValue, RenderedAttachmentCount> clearColors;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

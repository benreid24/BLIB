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
 * @tparam TAsset The GenericTargetAsset type to create
 * @ingroup Renderer
 */
template<typename TAsset>
class GenericTargetProvider : public rg::AssetProvider {
public:
    using Traits                                           = GenericTargetAssetTraits<TAsset>;
    static constexpr std::uint32_t RenderedAttachmentCount = Traits::RenderedAttachmentCount;

    /**
     * @brief Creates the provider
     *
     * @param imageUsages How the attachments will be used
     */
    GenericTargetProvider(bool terminal,
                          const std::array<VkClearValue, RenderedAttachmentCount>& clearColors)
    : terminal(terminal)
    , clearColors(clearColors) {}

    /**
     * @brief Creates a new generic target asset
     *
     * @param tag The tag to create the asset for
     * @return The newly created asset
     */
    virtual rg::Asset* create(std::string_view tag) override {
        return new TAsset(tag, terminal, clearColors);
    }

private:
    bool terminal;
    std::array<VkClearValue, RenderedAttachmentCount> clearColors;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

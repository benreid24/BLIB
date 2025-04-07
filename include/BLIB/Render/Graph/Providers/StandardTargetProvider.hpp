#ifndef BLIB_RENDER_GRAPH_PROVIDERS_STANDARDTARGETPROVIDER_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_STANDARDTARGETPROVIDER_HPP

#include <BLIB/Render/Graph/AssetProvider.hpp>
#include <BLIB/Render/Graph/Assets/StandardTargetAsset.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Provider for StandardTargetAsset assets
 *
 * @tparam RenderPassId The id of the render pass to use
 * @tparam ColorFormat The color format of the render target
 * @ingroup Renderer
 */
template<std::uint32_t RenderPassId, VkFormat ColorFormat>
class StandardAssetProvider : public rg::AssetProvider {
public:
    /**
     * @brief Creates the provider
     */
    StandardAssetProvider() = default;

private:
    virtual rg::Asset* create(std::string_view tag) override {
        return new StandardTargetAsset<RenderPassId, ColorFormat>(tag);
    }
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

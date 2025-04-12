#ifndef BLIB_RENDER_GRAPH_PROVIDERS_SIMPLEASSETPROVIDER_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_SIMPLEASSETPROVIDER_HPP

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
 * @brief Provider for StandardTargetAsset assets or other simple assets that only take the tag as
 *        the constructor argument
 *
 * @tparam TAsset The asset type to create
 * @ingroup Renderer
 */
template<typename TAsset>
class SimpleAssetProvider : public rg::AssetProvider {
public:
    /**
     * @brief Creates the provider
     */
    SimpleAssetProvider() = default;

private:
    virtual rg::Asset* create(std::string_view tag) override { return new TAsset(tag); }
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

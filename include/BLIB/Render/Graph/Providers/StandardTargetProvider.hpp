#ifndef BLIB_RENDER_GRAPH_PROVIDERS_STANDARDTARGETPROVIDER_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_STANDARDTARGETPROVIDER_HPP

#include <BLIB/Render/Graph/AssetProvider.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Provider for StandardTargetAsset assets
 *
 * @ingroup Renderer
 */
class StandardAssetProvider : public rg::AssetProvider {
public:
    /**
     * @brief Creates the provider
     */
    StandardAssetProvider() = default;

private:
    virtual rg::Asset* create() override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

#ifndef BLIB_RENDER_GRAPH_ASSETPROVIDER_HPP
#define BLIB_RENDER_GRAPH_ASSETPROVIDER_HPP

#include <string_view>

namespace bl
{
namespace rc
{
namespace rg
{
class Asset;

/**
 * @brief Base class for an asset provider. AssetProviders are used by AssetFactory
 *
 * @ingroup Renderer
 */
class AssetProvider {
public:
    /**
     * @brief Destroys the provider
     */
    virtual ~AssetProvider() = default;

    /**
     * @brief Create a new asset instance of the provided type
     *
     * @param tag The tag that the asset is being created for
     * @return A pointer to the new asset
     */
    virtual Asset* create(std::string_view tag) = 0;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif

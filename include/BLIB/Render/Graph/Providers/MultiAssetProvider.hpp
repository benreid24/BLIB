#ifndef BLIB_RENDER_GRAPH_PROVIDERS_MULTIASSETPROVIDER_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_MULTIASSETPROVIDER_HPP

#include <BLIB/Render/Graph/AssetProvider.hpp>
#include <BLIB/Render/Graph/MultiAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Provider wrapper for MultiAsset
 *
 * @tparam TProvider The underlying provider to use to create the assets
 * @tparam TAsset The asset type to create a MultiAsset of
 * @tparam N How many underlying assets should be created
 */
template<typename TProvider, typename TAsset, std::uint32_t N>
class MultiAssetProvider : public rg::AssetProvider {
public:
    /**
     * @brief Creates the multi asset provider
     *
     * @tparam ...TArgs Argument types to the underlying provider's constructor
     * @param terminal Whether the created assets should be terminal
     * @param ...args Arguments to the underlying provider's constructor
     */
    template<typename... TArgs>
    MultiAssetProvider(bool terminal, TArgs&&... args);

    /**
     * @brief Creates a new multi asset
     *
     * @param tag The tag to create the asset with
     * @return A pointer to the new asset
     */
    virtual rg::Asset* create(std::string_view tag) override;

private:
    TProvider provider;
    bool terminal;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TProvider, typename TAsset, std::uint32_t N>
template<typename... TArgs>
MultiAssetProvider<TProvider, TAsset, N>::MultiAssetProvider(bool terminal, TArgs&&... args)
: provider(std::forward<TArgs>(args)...)
, terminal(terminal) {}

template<typename TProvider, typename TAsset, std::uint32_t N>
rg::Asset* MultiAssetProvider<TProvider, TAsset, N>::create(std::string_view tag) {
    return new rg::MultiAsset<TAsset, N>(tag, terminal, provider);
}

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

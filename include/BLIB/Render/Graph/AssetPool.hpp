#ifndef BLIB_RENDER_GRAPH_ASSETPOOL_HPP
#define BLIB_RENDER_GRAPH_ASSETPOOL_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Graph/AssetFactory.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
class GraphAssetPool;

/**
 * @brief Observer level asset pool that owns assets directly
 *
 * @ingroup Renderer
 */
class AssetPool {
public:
    /**
     * @brief Creates a new asset pool
     *
     * @param factory The AssetFactory to use
     */
    AssetPool(AssetFactory& factory);

    /**
     * @brief Destroys all contained assets
     */
    void cleanup();

    /**
     * @brief Destroys assets that are not referenced by anything
     */
    void releaseUnused();

    /**
     * @brief Fetches an existing asset from the pool
     *
     * @param tag The tag of the asset to fetch
     * @param requester The pool requesting the asset
     * @return A pointer to an asset with the given tag, nullptr if none exist
     */
    Asset* getAsset(std::string_view tag, GraphAssetPool* requester);

    /**
     * @brief Gets or creates an asset of the given tag for the given pool
     *
     * @param tag The tag to get or create an asset for
     * @param requester The pool requesting the asset
     * @return A pointer to the asset to use
     */
    Asset* getOrCreateAsset(std::string_view tag, GraphAssetPool* requester);

    /**
     * @brief Manually puts an asset into the pool, bypassing the AssetFactory
     *
     * @tparam T The asset type to add
     * @tparam ...TArgs Argument types to the asset's constructor
     * @param tag The asset's tag
     * @param ...args Arguments to the asset's constructor
     * @return A pointer to the newly added asset
     */
    template<typename T, typename... TArgs>
    T* putAsset(std::string_view tag, TArgs&&... args) {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

        T* asset = new T(std::forward<TArgs>(args)...);
        assets[tag].emplace_back(asset);
        return asset;
    }

    /**
     * @brief Manually puts an asset into the pool, bypassing the AssetFactory. Replaces the
     *        existing assets with the same tag
     *
     * @tparam T The asset type to add
     * @tparam ...TArgs Argument types to the asset's constructor
     * @param tag The asset's tag
     * @param ...args Arguments to the asset's constructor
     * @return A pointer to the newly added asset
     */
    template<typename T, typename... TArgs>
    T* replaceAsset(std::string_view tag, TArgs&&... args) {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

        T* asset      = new T(std::forward<TArgs>(args)...);
        const auto it = assets.try_emplace(tag).first;
        it->second.clear();
        it->second.emplace_back(asset);
        return asset;
    }

private:
    AssetFactory& factory;
    std::unordered_map<std::string_view, std::vector<std::unique_ptr<Asset>>> assets;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif

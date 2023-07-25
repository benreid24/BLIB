#ifndef BLIB_RENDER_GRAPH_ASSETPOOL_HPP
#define BLIB_RENDER_GRAPH_ASSETPOOL_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Graph/AssetFactory.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace rc
{
class Observer;

namespace rgi
{
class FramebufferAsset;
}

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
     * @param observer The observer that the pool belongs to
     */
    AssetPool(AssetFactory& factory, Observer* observer);

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
     * @param ...args Arguments to the asset's constructor
     * @return A pointer to the newly added asset
     */
    template<typename T, typename... TArgs>
    T* putAsset(TArgs&&... args) {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

        T* asset = new T(std::forward<TArgs>(args)...);
        assets[asset->getTag()].emplace_back(asset);
        static_cast<Asset*>(asset)->external = true;
        bucketAsset(asset);
        return asset;
    }

    /**
     * @brief Manually puts an asset into the pool, bypassing the AssetFactory. Replaces the
     *        existing assets with the same tag
     *
     * @tparam T The asset type to add
     * @tparam ...TArgs Argument types to the asset's constructor
     * @param ...args Arguments to the asset's constructor
     * @return A pointer to the newly added asset
     */
    template<typename T, typename... TArgs>
    T* replaceAsset(TArgs&&... args) {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

        T* asset                             = new T(std::forward<TArgs>(args)...);
        static_cast<Asset*>(asset)->external = true;
        const auto it                        = assets.try_emplace(asset->getTag()).first;
        for (auto& a : it->second) { unbucketAsset(a.get()); }
        it->second.clear();
        it->second.emplace_back(asset);
        bucketAsset(asset);
        return asset;
    }

    /**
     * @brief Intended to be called by observer when it resizes
     *
     * @param newSize The new size of the observer render region
     */
    void notifyResize(glm::u32vec2 newSize);

    /**
     * @brief Resets resources used by the given pool
     *
     * @param pool The pool to reset resources for
     */
    void reset(GraphAssetPool* pool);

private:
    AssetFactory& factory;
    Observer* observer;
    std::unordered_map<std::string_view, std::vector<std::unique_ptr<Asset>>> assets;
    std::vector<rgi::FramebufferAsset*> framebufferAssets;

    void bucketAsset(Asset* asset);
    void unbucketAsset(Asset* asset);
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif

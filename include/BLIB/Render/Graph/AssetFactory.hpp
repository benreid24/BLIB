#ifndef BLIB_RENDER_GRAPH_ASSETFACTORY_HPP
#define BLIB_RENDER_GRAPH_ASSETFACTORY_HPP

#include <BLIB/Render/Graph/AssetProvider.hpp>
#include <initializer_list>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
/**
 * @brief Factory for render graph assets. Manages the set of providers keyed on tag
 *
 * @ingroup Renderer
 */
class AssetFactory {
public:
    /**
     * @brief Adds a new asset provider to the factory
     *
     * @tparam T The type of provider to add
     * @tparam ...TArgs Argument types to the provider's constructor
     * @param tag The tag that the provider creates assets for
     * @param ...args Arguments to the provider's constructor
     */
    template<typename T, typename... TArgs>
    void addProvider(std::string_view tag, TArgs&&... args) {
        auto& p = assetProviders.emplace_back(std::make_unique<T>(std::forward<TArgs>(args)...));
        providers.try_emplace(tag, p.get());
    }

    /**
     * @brief Adds a single asset provider for multiple tags
     *
     * @tparam T The type of provider to add
     * @tparam ...TArgs Argument types to the provider's constructor
     * @param tags The tags to assign the provider to
     * @param ...args Arguments to the provider's constructor
     */
    template<typename T, typename... TArgs>
    void addProvider(std::initializer_list<std::string_view> tags, TArgs&&... args) {
        auto& p = assetProviders.emplace_back(std::make_unique<T>(std::forward<TArgs>(args)...));
        for (const auto& tag : tags) { providers.try_emplace(tag, p.get()); }
    }

    /**
     * @brief Creates a new asset from the given tag
     *
     * @param tag The tag of the asset to create
     * @return The new asset
     */
    Asset* createAsset(std::string_view tag);

private:
    std::vector<std::unique_ptr<AssetProvider>> assetProviders;
    std::unordered_map<std::string_view, AssetProvider*> providers;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif

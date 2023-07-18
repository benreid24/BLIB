#include <BLIB/Render/Graph/AssetPool.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
AssetPool::AssetPool(AssetFactory& factory)
: factory(factory) {}

void AssetPool::cleanup() { assets.clear(); }

void AssetPool::releaseUnused() {
    for (auto& pair : assets) {
        std::erase_if(pair.second,
                      [](std::unique_ptr<Asset>& asset) { return asset->refCount == 0; });
    }
}

Asset* AssetPool::getAsset(std::string_view tag) {
    const auto it = assets.find(tag);
    return it != assets.end() && !it->second.empty() ? it->second.front().get() : nullptr;
}

Asset* AssetPool::getOrCreateAsset(std::string_view tag, GraphAssetPool* requester) {
    auto& set = assets[tag];
    for (auto& asset : set) {
        if (!asset->isOwnedBy(requester)) {
            asset->addOwner(requester);
            return asset.get();
        }
    }
    Asset* asset = factory.createAsset(tag);
    set.emplace_back(asset);
    asset->addOwner(requester);
    return asset;
}

} // namespace rg
} // namespace rc
} // namespace bl

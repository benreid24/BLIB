#include <BLIB/Render/Graph/GraphAssetPool.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
GraphAssetPool::GraphAssetPool(AssetPool& pool)
: pool(pool) {}

GraphAsset* GraphAssetPool::getAsset(std::string_view tag) {
    Asset* asset = pool.getAsset(tag, this);
    if (asset) {
        auto& set = assets[tag];
        return &set.emplace_back(asset);
    }
    return nullptr;
}

GraphAsset* GraphAssetPool::createAsset(std::string_view tag, Task* creator) {
    auto& set         = assets[tag];
    GraphAsset& asset = set.emplace_back(pool.getOrCreateAsset(tag, this));
    asset.outputtedBy = creator;
    return &asset;
}

void GraphAssetPool::reset() { assets.clear(); }

} // namespace rg
} // namespace rc
} // namespace bl

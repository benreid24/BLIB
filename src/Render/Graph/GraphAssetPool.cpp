#include <BLIB/Render/Graph/GraphAssetPool.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <stdexcept>

namespace bl
{
namespace rc
{
namespace rg
{
GraphAssetPool::GraphAssetPool(AssetPool& pool)
: pool(pool) {}

GraphAsset* GraphAssetPool::getFinalOutput() {
    auto& set = assets[AssetTags::FinalFrameOutput];
    if (set.size() != 1) {
        throw std::runtime_error("Swap frame output is missing from asset pool");
    }
    return &set.front();
}

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

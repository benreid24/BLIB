#include <BLIB/Render/Graph/AssetPool.hpp>

#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>

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
        std::erase_if(pair.second, [this](std::unique_ptr<Asset>& asset) {
            if (asset->refCount == 0) {
                unbucketAsset(asset.get());
                return true;
            }
            return false;
        });
    }
}

Asset* AssetPool::getAsset(std::string_view tag, GraphAssetPool* requester) {
    const auto it = assets.find(tag);
    if (it != assets.end()) {
        for (auto& asset : it->second) {
            if (!asset->isOwnedBy(requester)) {
                asset->addOwner(requester);
                return asset.get();
            }
        }
    }
    return nullptr;
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
    bucketAsset(asset);
    return asset;
}

void AssetPool::notifyResize(glm::u32vec2 newSize) {
    for (rgi::FramebufferAsset* asset : framebufferAssets) { asset->notifyResize(newSize); }
}

void AssetPool::bucketAsset(Asset* asset) {
    rgi::FramebufferAsset* fba = dynamic_cast<rgi::FramebufferAsset*>(asset);
    if (fba) { framebufferAssets.emplace_back(fba); }
}

void AssetPool::unbucketAsset(Asset* asset) {
    std::erase_if(framebufferAssets, [asset](rgi::FramebufferAsset* fba) {
        return static_cast<Asset*>(fba) == asset;
    });
}

} // namespace rg
} // namespace rc
} // namespace bl

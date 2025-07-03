#include <BLIB/Render/Graph/AssetPool.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
AssetPool::AssetPool(AssetFactory& factory, RenderTarget* observer)
: factory(factory)
, observer(observer) {}

void AssetPool::cleanup() { assets.clear(); }

void AssetPool::releaseUnused() {
    for (auto& pair : assets) {
        std::erase_if(pair.second, [this](std::unique_ptr<Asset>& asset) {
            if (asset->refCount == 0 && !asset->isExternal()) { return true; }
            return false;
        });
    }
}

Asset* AssetPool::getAsset(std::string_view tag, GraphAssetPool* requester) {
    const auto it = assets.find(tag);
    if (it != assets.end()) {
        if (!it->second.empty() && (requester == nullptr || it->second.front()->isExternal())) {
            return it->second.front().get();
        }
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
    return asset;
}

void AssetPool::notifyResize(glm::u32vec2 newSize) {
    for (auto& assetList : assets) {
        for (auto& asset : assetList.second) {
            if (asset->isCreated()) { asset->onResize(newSize); }
        }
    }
}

void AssetPool::reset(GraphAssetPool* gp) {
    for (auto& as : assets) {
        for (auto& a : as.second) { a->removeOwner(gp); }
    }
}

void AssetPool::startFrame() {
    for (auto& as : assets) {
        for (auto& a : as.second) { a->reset(); }
    }
}

} // namespace rg
} // namespace rc
} // namespace bl

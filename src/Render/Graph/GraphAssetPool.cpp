#include <BLIB/Render/Graph/GraphAssetPool.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <stdexcept>

namespace bl
{
namespace rc
{
namespace rg
{
GraphAssetPool::GraphAssetPool(Renderer& renderer, AssetPool& pool, RenderTarget* owner,
                               Scene* scene)
: owner(owner)
, scene(scene)
, pool(pool) {
    emitter.connect(renderer.getSignalChannel());
}

AssetRef GraphAssetPool::getAssetForAsset(std::string_view tag) {
    auto it = assets.find(tag);
    if (it != assets.end() && !it->second.empty()) {
        if (it->second.size() > 1) {
            BL_LOG_WARN << "More than one asset found for asset dependency: " << tag
                        << ". Choosing first";
        }
        return it->second.front().asset;
    }
    if (it == assets.end()) { it = assets.try_emplace(tag).first; }
    GraphAsset& ga = it->second.emplace_back(pool.getOrCreateAsset(tag, this));
    return ga.asset;
}

GraphAsset* GraphAssetPool::getFinalOutput() {
    auto& set = assets[AssetTags::FinalFrameOutput];
    if (set.size() != 1) {
        throw std::runtime_error("Swap frame output is missing from asset pool");
    }
    return &set.front();
}

GraphAsset* GraphAssetPool::getAssetForOutput(std::string_view tag, Task* task) {
    Asset* asset = pool.getAsset(tag, task != nullptr ? this : nullptr);
    if (asset) {
        // dont return non-external assets for inputs
        if (!task && !asset->isExternal()) { return nullptr; }

        auto& set            = assets[tag];
        const bool createNew = !asset->isExternal() || set.empty();
        GraphAsset* ga       = createNew ? &set.emplace_back(asset) : &set.front();
        if (createNew) {
            emitter.emit<event::SceneGraphAssetCreated>(
                {.target = owner, .scene = scene, .asset = ga});
        }
        return ga;
    }
    return nullptr;
}

GraphAsset* GraphAssetPool::getAssetForSharedOutput(
    std::string_view tag, const decltype(TaskOutput::sharedWith)& sharedWith) {
    const auto it = assets.find(tag);
    if (it != assets.end()) {
        for (auto& asset : it->second) {
            for (auto* other : asset.outputtedBy) {
                for (auto& tag : sharedWith) {
                    if (other->getId() == tag) { return &asset; }
                }
            }
        }
    }
    return nullptr;
}

GraphAsset* GraphAssetPool::getAssetForInput(std::string_view tag) {
    // search for existing asset before falling back onto pool
    const auto it = assets.find(tag);
    if (it != assets.end() && it->second.size() == 1) { return &it->second.front(); }

    // fallback into pool to find external asset
    return getAssetForOutput(tag, nullptr);
}

GraphAsset* GraphAssetPool::createAsset(std::string_view tag) {
    auto& set         = assets[tag];
    GraphAsset& asset = set.emplace_back(pool.getOrCreateAsset(tag, this));
    emitter.emit<event::SceneGraphAssetCreated>({.target = owner, .scene = scene, .asset = &asset});
    return &asset;
}

void GraphAssetPool::reset() {
    pool.reset(this);
    assets.clear();
}

} // namespace rg
} // namespace rc
} // namespace bl

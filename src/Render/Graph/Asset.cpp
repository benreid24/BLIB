#include <BLIB/Render/Graph/Asset.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Graph/AssetPool.hpp>
#include <BLIB/Render/RenderTarget.hpp>
#include <algorithm>

namespace bl
{
namespace rc
{
namespace rg
{
Asset::Asset(std::string_view tag)
: tag(tag)
, created(false)
, refCount(0)
, mode(InputMode::Unset)
, external(false) {}

void Asset::addDependency(std::string_view depTag) {
    if (created) {
        BL_LOG_ERROR << "Cannot add dependency after asset creation";
        return;
    }

    depTags.emplace_back(depTag);
}

Asset* Asset::getDependency(std::string_view depTag) {
    for (unsigned int i = 0; i < depTags.size(); ++i) {
        if (depTags[i] == depTag) {
            if (i < dependencies.size()) { return &dependencies[i].get(); }
            BL_LOG_ERROR << "Dependency tag " << depTag
                         << " not found in dependencies, but was registered";
            return nullptr;
        }
    }
    BL_LOG_ERROR << "Dependency tag " << depTag << " not found in asset dependencies";
    return nullptr;
}

Asset* Asset::getDependency(unsigned int index) { return &dependencies[index].get(); }

bool Asset::create(engine::Engine& engine, Renderer& renderer, RenderTarget* observer) {
    if (!created) {
        created = true;
        dependencies.reserve(depTags.size());
        for (const auto& depTag : depTags) {
            Asset* dep = observer->getAssetPool().getOrCreateAsset(depTag, nullptr);
            if (!dep) { BL_LOG_ERROR << "Failed to find dependency asset with tag: " << depTag; }
            dependencies.emplace_back(dep);
        }
        doCreate(engine, renderer, observer);
        return true;
    }
    return false;
}

void Asset::prepareForInput(const ExecutionContext& ctx) {
    if (mode != InputMode::Input) {
        mode = InputMode::Input;
        doPrepareForInput(ctx);
        for (AssetRef& dep : dependencies) { dep->prepareForInput(ctx); }
    }
}

void Asset::startOutput(const ExecutionContext& ctx) {
    if (mode != InputMode::OutputStart) {
        mode = InputMode::OutputStart;
        for (AssetRef& dep : dependencies) { dep->startOutput(ctx); }
        doStartOutput(ctx);
    }
}

void Asset::endOutput(const ExecutionContext& ctx) {
    if (mode != InputMode::OutputEnd) {
        mode = InputMode::OutputEnd;
        for (AssetRef& dep : dependencies) { dep->endOutput(ctx); }
        doEndOutput(ctx);
    }
}

bool Asset::isOwnedBy(GraphAssetPool* pool) {
    return std::find(owners.begin(), owners.end(), pool) != owners.end();
}

void Asset::addOwner(GraphAssetPool* pool) {
    owners.emplace_back(pool);
    for (AssetRef& dep : dependencies) { dep->addOwner(pool); }
}

void Asset::removeOwner(GraphAssetPool* pool) {
    const auto it = std::find(owners.begin(), owners.end(), pool);
    if (it != owners.end()) {
        owners.erase(it);
        for (AssetRef& dep : dependencies) { dep->removeOwner(pool); }
    }
}

void Asset::reset() {
    mode = InputMode::Unset;
    onReset();
}

void Asset::onResize(glm::u32vec2) {}

} // namespace rg
} // namespace rc
} // namespace bl

#include <BLIB/Assets/Repository.hpp>

namespace bl
{
namespace as
{
Repository::Repository()
: mode(Mode::Editor) {
    drivers.reserve(16);
}

const std::vector<RepoDependency>& Repository::getDependencies(util::UUID uuid) const {
    std::shared_lock lock(assetMutex);
    auto it = assets.find(uuid);
    if (it == assets.end()) {
        BL_LOG_ERROR << "Attempted to get dependencies for asset with UUID " << uuid.toString()
                     << " but it does not exist";
        static const std::vector<RepoDependency> empty;
        return empty;
    }
    return it->second.dependencies;
}

Ref Repository::getAsset(util::UUID uuid, State desiredState) {
    std::shared_lock lock(assetMutex);
    auto it = assets.find(uuid);
    if (it == assets.end()) {
        BL_LOG_ERROR << "Attempted to get asset with UUID " << uuid.toString()
                     << " but it does not exist";
        return Ref();
    }
    // TODO - when bundling is added we will have an index of known but unloaded assets to check
    RepoAsset& stored = it->second;
    if (stored.asset.getState() < desiredState) {
        if (!stored.asset.load()) { return Ref(); }
    }
    return Ref(this, &stored);
}

void Repository::registerDependency(util::UUID uuid, std::string_view tag, util::UUID dependency) {
    std::unique_lock lock(assetMutex);
    auto it = assets.find(uuid);
    if (it == assets.end()) {
        BL_LOG_ERROR << "Attempted to register dependency for asset with UUID " << uuid.toString()
                     << " but it does not exist";
        return;
    }
    RepoAsset& stored = it->second;
    stored.dependencies.push_back(RepoDependency{dependency, tag});
}

void Repository::queueUnload(util::UUID uuid) {
    std::unique_lock lock(unloadQueueMutex);
    unloadQueue.push_back(uuid);
}

} // namespace as
} // namespace bl

#include <BLIB/Assets/Repository.hpp>

#include <BLIB/Serialization.hpp>

namespace bl
{
namespace as
{
namespace
{
struct Manifest {
    std::vector<util::UUID> assetUuids;
};
} // namespace
} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::Manifest> : public SerializableObjectBase {
    SerializableField<1, as::Manifest, std::vector<util::UUID>> assetUuids;

    SerializableObject()
    : SerializableObjectBase("AssetManifest")
    , assetUuids("assetUuids", *this, &as::Manifest::assetUuids,
                 SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace as
{

Repository::Repository(Mode mode, const std::string& path)
: mode(mode)
, assetDirectory(path) {
    drivers.reserve(16);
}

Ref Repository::createAsset(std::string_view type, const std::string& name,
                            const CreateContext::CustomData& createData) {
    if (!getDriver(type)) {
        BL_LOG_ERROR << "Attempted to create asset with type " << type
                     << " but no driver is registered for that type";
        return Ref();
    }

    std::unique_lock assetLock(assetMutex);
    util::UUID uuid = util::UUID::generate();
    auto it         = assets.try_emplace(uuid, *this).first;
    it->second.getMetadata().setDisplayName(name);
    it->second.type = type;
    if (!it->second.create(createData)) {
        BL_LOG_ERROR << "Failed to create new asset '" << name << "' of type " << type;
        return Ref();
    }

    return Ref(this, &it->second);
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
    Asset& stored = it->second;
    if (stored.getState() < desiredState) {
        if (!stored.load()) { return Ref(); }
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
    Asset& stored = it->second;
    stored.dependencies.push_back(RepoDependency{dependency, std::string(tag)});
}

void Repository::queueUnload(util::UUID uuid) {
    std::unique_lock lock(unloadQueueMutex);
    unloadQueue.push_back(uuid);
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

detail::DriverBase* Repository::getDriver(std::string_view tag) {
    std::shared_lock lock(driverMutex);
    auto it = driversByName.find(tag);
    if (it == driversByName.end()) { return nullptr; }
    return it->second;
}

bool Repository::saveRepository() {
    std::unique_lock lock(assetMutex);

    // ensure root dir exists
    if (!util::FileUtil::createDirectory(assetDirectory)) {
        BL_LOG_ERROR << "Failed to create asset directory at " << assetDirectory;
        return false;
    }

    // write manifest
    const std::string manifestPath = util::FileUtil::joinPath(assetDirectory, "manifest.json");
    Manifest manifest;
    manifest.assetUuids.reserve(assets.size());
    for (const auto& pair : assets) { manifest.assetUuids.emplace_back(pair.first); }
    std::ofstream manifestFile(manifestPath);
    if (!serial::json::Serializer<Manifest>::serializeStream(manifestFile, manifest, 4, 0)) {
        BL_LOG_ERROR << "Failed to write asset manifest to " << manifestPath;
        return false;
    }
    manifestFile.close();

    // flush assets to disk
    bool allSuccess = true;
    for (auto& pair : assets) {
        if (!pair.second.saveEditor()) {
            BL_LOG_ERROR << "Failed to save asset for editor with UUID " << pair.first.toString();
            allSuccess = false;
        }
    }

    return allSuccess;
}

bool Repository::loadRepository() {
    // TODO
    return false;
}

bool Repository::exportRepository(const std::string& path) {
    // TODO
    return false;
}

void Repository::releaseUnused() {
    std::unique_lock lock(assetMutex);
    std::unique_lock unloadLock(unloadQueueMutex);
    for (const util::UUID& uuid : unloadQueue) {
        auto it = assets.find(uuid);
        if (it == assets.end()) {
            BL_LOG_ERROR << "Attempted to unload asset with UUID " << uuid.toString()
                         << " but it does not exist";
            continue;
        }
        Asset& stored = it->second;
        if (!stored.unload()) {
            BL_LOG_WARN << "Failed to unload asset with UUID " << uuid.toString();
        }
    }
}

} // namespace as
} // namespace bl

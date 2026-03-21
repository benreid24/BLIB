#include <BLIB/Assets/Repository.hpp>

#include <BLIB/Assets/EditorPaths.hpp>
#include <BLIB/Serialization.hpp>

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<as::Repository> : public SerializableObjectBase {
    SerializableField<1, as::Repository, std::unordered_map<util::UUID, as::Asset>> assets;

    SerializableObject()
    : SerializableObjectBase("AssetManifest")
    , assets("assets", *this, &as::Repository::assets, SerializableFieldBase::Required{}) {}
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
    it->second.uuid = uuid;
    it->second.type = type;
    if (!it->second.create(createData)) {
        BL_LOG_ERROR << "Failed to create new asset '" << name << "' of type " << type;
        assets.erase(it);
        return Ref();
    }

    return Ref(this, &it->second);
}

Ref Repository::getAsset(util::UUID uuid, State desiredState) {
    std::unique_lock lock(assetMutex);
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
    std::unique_lock lock(assetMutex);
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
    return saveRepositoryLocked();
}

bool Repository::saveRepositoryLocked() {
    // ensure root dir exists
    if (!util::FileUtil::createDirectory(assetDirectory)) {
        BL_LOG_ERROR << "Failed to create asset directory at " << assetDirectory;
        return false;
    }

    // write manifest
    const std::string manifestPath = util::FileUtil::joinPath(assetDirectory, "manifest.json");
    std::ofstream manifestFile(manifestPath);
    if (!serial::json::Serializer<Repository>::serializeStream(manifestFile, *this, 4, 0)) {
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
    std::unique_lock lock(assetMutex);
    std::unique_lock unloadLock(unloadQueueMutex);

    assets.clear();
    unloadQueue.clear();

    if (mode == Mode::Editor) {
        // TODO - should path loaded from depend on mode?
        const std::string manifestPath = util::FileUtil::joinPath(assetDirectory, "manifest.json");
        std::ifstream manifestFile(manifestPath);
        if (!manifestFile.is_open()) {
            BL_LOG_ERROR << "Failed to open asset manifest at " << manifestPath;
            return false;
        }
        if (!serial::json::Serializer<Repository>::deserializeStream(manifestFile, *this)) {
            BL_LOG_ERROR << "Failed to deserialize asset manifest at " << manifestPath;
            return false;
        }
        // check for missing asset files and search if moved
        std::unordered_map<std::string, std::string> foundAssetPaths;
        std::vector<util::UUID> missingAssets;
        for (auto& pair : assets) {
            const std::string assetPath =
                EditorPaths::getAssetMetadataFilePath(assetDirectory, pair.second);
            if (!util::FileUtil::exists(assetPath)) {
                BL_LOG_WARN << "Asset with UUID " << pair.first.toString()
                            << " is missing its metadata file at expected path " << assetPath;

                // find all asset metadata files
                if (foundAssetPaths.empty()) {
                    const std::vector<std::string> metadataFiles = util::FileUtil::listDirectory(
                        assetDirectory, std::string(EditorPaths::MetadataExtension), true);
                    for (const auto& path : metadataFiles) {
                        const std::string uuid = util::FileUtil::getBaseName(path);
                        foundAssetPaths[uuid]  = path;
                    }
                }

                // see if our missing asset was moved or deleted
                const auto it = foundAssetPaths.find(pair.first.toString());
                if (it != foundAssetPaths.end()) {
                    BL_LOG_INFO << "Found metadata file for asset with UUID "
                                << pair.first.toString() << " at " << it->second
                                << ". Updating asset path and display name";
                    const std::string displayName = util::FileUtil::getFolder(it->second);
                    std::string folder            = it->second;
                    // remove folder + filename + assetDirectory
                    const std::size_t rmStart =
                        assetDirectory.size() + EditorPaths::RootPath.size() + 2;
                    const std::size_t rmSize = rmStart + displayName.size() + 1 +
                                               EditorPaths::MetadataExtension.size() +
                                               util::UUID::StringLength;
                    folder = folder.substr(rmStart, folder.size() - rmSize);

                    pair.second.getMetadata().setDisplayName(displayName);
                    pair.second.getMetadata().setPath(folder);
                }
                else {
                    BL_LOG_ERROR << "Asset with UUID " << pair.first.toString()
                                 << " is missing its metadata file and could not be found in "
                                    "asset directory. Asset will be deleted";
                    missingAssets.push_back(pair.first);
                }
            }
        }

        // delete missing assets
        if (!missingAssets.empty()) {
            BL_LOG_WARN << "Deleting " << missingAssets.size() << " missing assets";
            for (const util::UUID& uuid : missingAssets) { assets.erase(uuid); }
        }
    }
    else {
        // TODO - implement bundle loading
        return false;
    }

    for (auto& pair : assets) {
        const bool hasAllDeps = pair.second.initAfterDeserialize(*this);
        if (pair.second.getMetadata().getIsAutoLoaded()) {
            if (hasAllDeps) {
                if (!pair.second.load()) {
                    BL_LOG_ERROR << "Failed to auto load auto-load asset with UUID "
                                 << pair.first.toString();
                }
            }
            else {
                BL_LOG_ERROR << "Skipping auto-load of asset " << pair.first.toString()
                             << " due to missing dependencies";
            }
        }
    }

    return true;
}

Asset* Repository::getDependencyForInit(util::UUID uuid) {
    auto it = assets.find(uuid);
    if (it == assets.end()) { return nullptr; }
    return &it->second;
}

bool Repository::exportRepository(const std::string& path) {
    // TODO
    return false;
}

void Repository::releaseUnused() {
    std::unique_lock lock(assetMutex);
    std::unique_lock unloadLock(unloadQueueMutex);

    if (mode == Mode::Editor) {
        if (!saveRepositoryLocked()) {
            BL_LOG_WARN << "Failed to save repository before releasing unused assets";
        }
    }

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

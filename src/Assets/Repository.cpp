#include <BLIB/Assets/Repository.hpp>

#include <BLIB/Assets/Drivers/ImageDriver.hpp>
#include <BLIB/Assets/Drivers/MusicDriver.hpp>
#include <BLIB/Assets/Drivers/SoundDriver.hpp>
#include <BLIB/Assets/EditorPaths.hpp>
#include <BLIB/Serialization.hpp>

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<as::Repository> : public SerializableObjectBase {
    SerializableField<1, as::Repository, std::unordered_map<util::UUID, as::Asset>> assets;
    SerializableField<2, as::Repository, std::unordered_map<std::string, as::SourceLink>>
        sourceLinks;

    SerializableObject()
    : SerializableObjectBase("AssetManifest")
    , assets("assets", *this, &as::Repository::assets, SerializableFieldBase::Required{})
    , sourceLinks("sourceLinks", *this, &as::Repository::sourceLinks,
                  SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace as
{
namespace
{
std::string makeStaticPath(std::string_view type) {
    constexpr std::string_view prefix = "Static - ";
    std::string path;
    path.reserve(prefix.size() + type.size());
    path += prefix;
    path += type;
    return path;
}
} // namespace

Repository::Repository(Mode mode, const std::string& path)
: mode(mode)
, assetDirectory(path) {
    drivers.reserve(16);

    // register built-in drivers
    registerDriver<asi::ImageDriver>(asi::ImageDriver::TypeName);
    registerDriver<asi::SoundDriver>(asi::SoundDriver::TypeName);
    registerDriver<asi::MusicDriver>(asi::MusicDriver::TypeName);
}

Ref Repository::createAsset(std::string_view type, const std::string& name,
                            const CreateContext::CreateData& createData) {
    return createAssetShared(type, name, createData, true);
}

Ref Repository::createAssetShared(std::string_view type, const std::string& name,
                                  const CreateContext::CreateData& createData, bool sync) {
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
        util::FileUtil::deleteDirectory(EditorPaths::getAssetPath(assetDirectory, it->second));
        assets.erase(it);
        return Ref();
    }

    if (!createData.getPath().empty()) {
        it->second.getMetadata().setSourceFileInfo(createData.getPath());
    }

    if (mode == Mode::Editor && sync) {
        it->second.markReadyForAutoSync();

        if (!it->second.saveEditor()) {
            BL_LOG_WARN << "Failed to save newly created asset '" << name << "' of type " << type
                        << " to disk";
        }
        if (!writeManifestLocked()) {
            BL_LOG_WARN << "Failed to save asset manifest after creating new asset '" << name
                        << "' of type " << type;
        }
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
    if (stored.getState() < desiredState && stored.getState() >= State::Failed) {
        if (!stored.load()) { return Ref(); }
    }
    return Ref(this, &stored);
}

Ref Repository::getAssetFromSourcePath(std::string_view type, const std::string& path,
                                       State desiredState) {
    std::unique_lock lock(sourceLinkMutex);

    auto it = sourceLinks.find(path);
    if (it == sourceLinks.end()) {
        Ref ref = createAssetShared(type, path, CreateContext::CreateData(path), false);
        if (!ref) { return ref; }
        sourceLinks.try_emplace(path, ref->getUUID(), path, type);
        ref->getMetadata().setPath(makeStaticPath(type));
        ref->markReadyForAutoSync();
        if (mode == Mode::Editor) {
            if (!writeManifestLocked()) {
                BL_LOG_WARN
                    << "Failed to save asset manifest after creating static asset with path "
                    << path << " and type " << type;
            }
            if (!ref.getAsset().saveEditor()) {
                BL_LOG_WARN << "Failed to save newly created static asset '" << path << "' of type "
                            << type << " to disk";
            }
        }
        return ref;
    }
    else {
        if (it->second.type != type) {
            BL_LOG_ERROR << "Attempted to get static asset with path " << path << " and type "
                         << type << " but a static asset with that path exists with type "
                         << it->second.type;
            return Ref();
        }
        return getAsset(it->second.uuid, desiredState);
    }
}

std::optional<util::UUID> Repository::findStaticAssetId(const std::string& path) const {
    std::unique_lock lock(sourceLinkMutex);

    auto it = sourceLinks.find(path);
    if (it == sourceLinks.end()) { return std::nullopt; }
    return it->second.uuid;
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

    if (mode == Mode::Editor) {
        if (!writeManifestLocked()) {
            BL_LOG_WARN << "Failed to save asset manifest after registering dependency "
                        << uuid.toString() << " -> " << dependency.toString() << " (" << tag << ")";
        }
    }
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

bool Repository::writeManifest() {
    std::unique_lock lock(assetMutex);
    return writeManifestLocked();
}

bool Repository::writeManifestLocked() {
    // ensure root dir exists
    if (!util::FileUtil::createDirectory(assetDirectory)) {
        BL_LOG_ERROR << "Failed to create asset directory at " << assetDirectory;
        return false;
    }

    // write manifest
    const std::string manifestPath = util::FileUtil::joinPath(assetDirectory, "manifest.json");
    stream::OutputStream manifestFile(manifestPath);
    if (!serial::json::Serializer<Repository>::serializeStream(manifestFile, *this, 4, 0)) {
        BL_LOG_ERROR << "Failed to write asset manifest to " << manifestPath;
        return false;
    }
    manifestFile.close();

    return true;
}

bool Repository::loadRepository() {
    std::unique_lock lock(assetMutex);
    std::unique_lock unloadLock(unloadQueueMutex);

    assets.clear();
    unloadQueue.clear();

    if (mode == Mode::Editor) {
        // TODO - should path loaded from depend on mode?
        const std::string manifestPath = util::FileUtil::joinPath(assetDirectory, "manifest.json");
        stream::InputStream manifestFile(manifestPath);
        if (!manifestFile.isValid()) {
            BL_LOG_ERROR << "Failed to open asset manifest at " << manifestPath;
            return false;
        }
        if (!serial::json::Serializer<Repository>::deserializeStream(manifestFile, *this)) {
            BL_LOG_ERROR << "Failed to deserialize asset manifest at " << manifestPath;
            return false;
        }

        // load asset metadata from filesystem to find missing or moved assets
        std::unordered_map<std::string, std::string> foundAssetPaths;
        const std::vector<std::string> metadataFiles = util::FileUtil::listDirectory(
            assetDirectory, std::string(EditorPaths::MetadataExtension), true);
        for (const auto& path : metadataFiles) {
            const std::string uuid = util::FileUtil::getBaseName(path);
            foundAssetPaths[uuid]  = path;
        }

        // find and relink moved assets
        for (auto& pair : assets) {
            const std::string assetPath =
                EditorPaths::getAssetMetadataFilePath(assetDirectory, pair.second);
            if (!util::FileUtil::exists(assetPath)) {
                BL_LOG_WARN << "Asset with UUID " << pair.first.toString()
                            << " is missing its metadata file at expected path " << assetPath;

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

                    if (!folder.empty()) {
                        if (folder.front() == '/' || folder.front() == '\\') { folder.erase(0, 1); }
                    }
                    if (!folder.empty()) {
                        if (folder.back() == '/' || folder.back() == '\\') { folder.pop_back(); }
                    }

                    pair.second.getMetadata().setDisplayName(displayName);
                    pair.second.getMetadata().setPath(folder);
                }
                else {
                    BL_LOG_ERROR << "Asset with UUID " << pair.first.toString()
                                 << " is missing its metadata file and could not be found in "
                                    "asset directory";
                    pair.second.state = State::Missing;
                }
            }
        }

        // search discovered assets for any unregistered and add them to manifest
        for (const auto& pair : foundAssetPaths) {
            const util::UUID uuid(pair.first);
            if (assets.find(uuid) == assets.end()) {
                BL_LOG_INFO << "Found unregistered asset with UUID " << pair.first << " at "
                            << pair.second << ". Adding to repository manifest";

                // deserialize metadata file
                stream::InputStream assetFile(pair.second);
                if (!assetFile.isValid()) {
                    BL_LOG_ERROR << "Failed to open asset file at " << pair.second;
                    continue;
                }

                // load asset info
                auto it = assets.try_emplace(uuid, *this).first;
                if (!serial::json::Serializer<Asset>::deserializeStream(assetFile, it->second)) {
                    BL_LOG_ERROR << "Failed to deserialize asset metadata at " << pair.second;
                    assets.erase(it);
                    continue;
                }

                // validate
                if (it->second.getUUID() != uuid) {
                    BL_LOG_ERROR << "Deserialized asset metadata at " << pair.second
                                 << " has mismatched UUID " << it->second.getUUID().toString()
                                 << " (expected " << uuid.toString() << ")";
                    assets.erase(it);
                    continue;
                }
            }
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

PersistentStream* Repository::getPersistentStream(util::UUID uuid, const std::string& path) {
    return streamCache.getStream(uuid, path);
}

} // namespace as
} // namespace bl

#include <BLIB/Assets/Repository.hpp>

#include <BLIB/Assets/Bundles/Manifest.hpp>
#include <BLIB/Assets/Bundles/RuntimePaths.hpp>
#include <BLIB/Assets/Drivers/Animation2DDriver.hpp>
#include <BLIB/Assets/Drivers/Animation2DSetDriver.hpp>
#include <BLIB/Assets/Drivers/Animation3DDriver.hpp>
#include <BLIB/Assets/Drivers/CubemapDriver.hpp>
#include <BLIB/Assets/Drivers/ImageDriver.hpp>
#include <BLIB/Assets/Drivers/MaterialDriver.hpp>
#include <BLIB/Assets/Drivers/ModelDriver.hpp>
#include <BLIB/Assets/Drivers/MusicDriver.hpp>
#include <BLIB/Assets/Drivers/PlaylistDriver.hpp>
#include <BLIB/Assets/Drivers/SoundDriver.hpp>
#include <BLIB/Assets/Drivers/TextureDriver.hpp>
#include <BLIB/Assets/EditorPaths.hpp>
#include <BLIB/Serialization.hpp>
#include <queue>

namespace bl
{
namespace as
{
struct EditorRuntimeData {
    std::unordered_map<util::UUID, as::Asset>* assets;
    std::unordered_map<std::string, std::unordered_map<std::string, as::SourceLink>>* sourceLinks;
    std::unordered_map<std::string, util::UUID>* keyToAsset;
};

struct GameRuntimeData {
    std::unordered_map<util::UUID, as::Asset>* assets;
    std::unordered_map<std::string, util::UUID>* keyToAsset;
    bdl::Manifest* bundleManifest;
};
} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::EditorRuntimeData> : public SerializableObjectBase {
    SerializableField<1, as::EditorRuntimeData, std::unordered_map<util::UUID, as::Asset>*> assets;
    SerializableField<
        2, as::EditorRuntimeData,
        std::unordered_map<std::string, std::unordered_map<std::string, as::SourceLink>>*>
        sourceLinks;
    SerializableField<3, as::EditorRuntimeData, std::unordered_map<std::string, util::UUID>*>
        keyToAsset;

    SerializableObject()
    : SerializableObjectBase("AssetManifest")
    , assets("assets", *this, &as::EditorRuntimeData::assets, SerializableFieldBase::Required{})
    , sourceLinks("sourceLinks", *this, &as::EditorRuntimeData::sourceLinks,
                  SerializableFieldBase::Required{})
    , keyToAsset("keyToAsset", *this, &as::EditorRuntimeData::keyToAsset,
                 SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<as::GameRuntimeData> : public SerializableObjectBase {
    SerializableField<1, as::GameRuntimeData, std::unordered_map<util::UUID, as::Asset>*> assets;
    // source links slot 2 ommitted
    SerializableField<3, as::GameRuntimeData, std::unordered_map<std::string, util::UUID>*>
        keyToAsset;
    SerializableField<4, as::GameRuntimeData, as::bdl::Manifest*> bundleManifest;

    SerializableObject()
    : SerializableObjectBase("AssetManifest")
    , assets("assets", *this, &as::GameRuntimeData::assets, SerializableFieldBase::Required{})
    , keyToAsset("keyToAsset", *this, &as::GameRuntimeData::keyToAsset,
                 SerializableFieldBase::Required{})
    , bundleManifest("bundleManifest", *this, &as::GameRuntimeData::bundleManifest,
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

Repository::~Repository() {
    // Need to force clear here so that refs in payloads do not attempt to release from partially
    // destroyed repository
    forceUnloadAll();
}

Repository::Repository(Mode mode, const std::string& path)
: mode(mode)
, assetDirectory(path)
, bundleRuntime(*this, path) {
    drivers.reserve(16);

    // register built-in drivers
    registerDriver<asi::ImageDriver>(asi::ImageDriver::TypeName);
    registerDriver<asi::SoundDriver>(asi::SoundDriver::TypeName);
    registerDriver<asi::MusicDriver>(asi::MusicDriver::TypeName);
    registerDriver<asi::TextureDriver>(asi::TextureDriver::TypeName);
    registerDriver<asi::CubemapDriver>(asi::CubemapDriver::TypeName);
    registerDriver<asi::MaterialDriver>(asi::MaterialDriver::TypeName);
    registerDriver<asi::Animation3DDriver>(asi::Animation3DDriver::TypeName);
    registerDriver<asi::ModelDriver>(asi::ModelDriver::TypeName);
    registerDriver<asi::PlaylistDriver>(asi::PlaylistDriver::TypeName);
    registerDriver<asi::Animation2DDriver>(asi::Animation2DDriver::TypeName);
    registerDriver<asi::Animation2DSetDriver>(asi::Animation2DSetDriver::TypeName);
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
        std::filesystem::path source = createData.getPath();
        if (util::FileUtil::isSubpath(createData.getPath())) {
            source = std::filesystem::proximate(createData.getPath());
        }
        else {
            BL_LOG_WARN << "Source file path '" << createData.getPath()
                        << "' is not a subpath of the current working directory. Storing absolute "
                           "path in asset metadata, which may cause issues if the asset is moved";
        }
        it->second.getMetadata().setSourceFileInfo(source.generic_string());
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

Ref Repository::getOrCreateAsset(std::string_view key, std::string_view type,
                                 const std::string& name,
                                 const CreateContext::CreateData& createData) {
    std::string keyStr(key);

    std::unique_lock lock(assetMutex);
    auto it = keyToAsset.find(keyStr);
    if (it != keyToAsset.end()) {
        auto asset = getAsset(it->second, State::Loaded);
        if (asset) { return asset; }
        BL_LOG_WARN << "Asset with key '" << key
                    << "' exists but failed to load, attempting to create new asset";
    }

    auto ref = createAsset(type, name, createData);
    if (ref) { keyToAsset.try_emplace(std::move(keyStr), ref.getUUID()); }
    return ref;
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

    auto& linksByType = sourceLinks[std::string(type)];
    auto it           = linksByType.find(path);
    if (it == linksByType.end()) {
        Ref ref = createAssetShared(type, path, CreateContext::CreateData(path), false);
        if (!ref) { return ref; }
        linksByType.try_emplace(path, ref->getUUID(), path, type);
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
            BL_LOG_ERROR << "Attempted to get asset with path " << path << " and type " << type
                         << " but an asset with that path exists with type " << it->second.type;
            return Ref();
        }
        return getAsset(it->second.uuid, desiredState);
    }
}

std::optional<util::UUID> Repository::findAssetIdFromSourcePath(std::string_view type,
                                                                const std::string& path) const {
    std::unique_lock lock(sourceLinkMutex);

    const auto& linksByType = sourceLinks.find(std::string(type));
    if (linksByType == sourceLinks.end()) { return std::nullopt; }
    auto it = linksByType->second.find(path);
    if (it == linksByType->second.end()) { return std::nullopt; }
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

    for (const auto& dep : stored.dependencies) {
        if (dep.uuid == dependency && dep.tag == tag) {
            BL_LOG_WARN << "Ignoring duplicate dependency with UUID " << dependency.toString()
                        << " and tag " << tag << " for asset " << uuid.toString();
            return;
        }
    }

    stored.dependencies.emplace_back(dependency, std::string(tag));

    if (mode == Mode::Editor) {
        if (!writeManifestLocked()) {
            BL_LOG_WARN << "Failed to save asset manifest after registering dependency "
                        << uuid.toString() << " -> " << dependency.toString() << " (" << tag << ")";
        }
    }
}

bool Repository::unregisterDependency(util::UUID uuid, std::string_view tag, util::UUID dep) {
    std::unique_lock lock(assetMutex);
    auto it = assets.find(uuid);
    if (it == assets.end()) {
        BL_LOG_ERROR << "Attempted to register dependency for asset with UUID " << uuid.toString()
                     << " but it does not exist";
        return false;
    }
    Asset& stored = it->second;
    for (auto it = stored.dependencies.begin(); it != stored.dependencies.end(); ++it) {
        if (it->tag == tag && it->uuid == uuid) {
            stored.dependencies.erase(it);
            if (mode == Mode::Editor) {
                if (!writeManifestLocked()) {
                    BL_LOG_WARN << "Failed to save asset manifest after unregistering dependency "
                                << uuid.toString() << " -> " << dep.toString() << " (" << tag
                                << ")";
                }
            }
            return true;
        }
    }
    return false;
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
    return getDriverLocked(tag);
}

detail::DriverBase* Repository::getDriverLocked(std::string_view tag) {
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
    const EditorRuntimeData data{
        .assets      = &assets,
        .sourceLinks = &sourceLinks,
        .keyToAsset  = &keyToAsset,
    };
    if (!serial::json::Serializer<EditorRuntimeData>::serializeStream(manifestFile, data, 4, 0)) {
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
        const std::string manifestPath = util::FileUtil::joinPath(assetDirectory, "manifest.json");
        stream::InputStream manifestFile(manifestPath);
        if (!manifestFile.isValid()) {
            BL_LOG_ERROR << "Failed to open asset manifest at " << manifestPath;
            return false;
        }

        EditorRuntimeData data{
            .assets      = &assets,
            .sourceLinks = &sourceLinks,
            .keyToAsset  = &keyToAsset,
        };
        if (!serial::json::Serializer<EditorRuntimeData>::deserializeStream(manifestFile, data)) {
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
                    const std::string assetFolder = util::FileUtil::getFolder(it->second);
                    auto parsed                   = EditorPaths::parseAssetFolderName(assetFolder);
                    const std::string displayName = parsed ? parsed->second : "RecoveredAsset";
                    std::string folder            = it->second;
                    // remove folder + filename + assetDirectory
                    const std::size_t rmStart = assetDirectory.size() + 1;
                    const std::size_t rmSize  = rmStart + assetFolder.size() + 1 +
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
        const std::string manifestPath =
            util::FileUtil::joinPath(assetDirectory, std::string(bdl::RuntimePaths::ManifestPath));
        stream::InputStream manifestFile(manifestPath);
        if (!manifestFile.isValid()) {
            BL_LOG_ERROR << "Failed to open asset manifest at " << manifestPath;
            return false;
        }

        GameRuntimeData data{.assets         = &assets,
                             .keyToAsset     = &keyToAsset,
                             .bundleManifest = &bundleRuntime.manifest};
        if (!serial::binary::Serializer<GameRuntimeData>::deserialize(manifestFile, data)) {
            BL_LOG_ERROR << "Failed to read asset manifest file";
            return false;
        }

        streamCache.useBundles(&bundleRuntime);
    }

    // auto load assets that are marked as such
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
    std::unique_lock assetLock(assetMutex);
    std::unique_lock driverLock(driverMutex);

    // create bundle directory
    if (!util::FileUtil::createDirectory(path)) {
        BL_LOG_ERROR << "Failed to create bundle directory at " << path;
        return false;
    }

    // build set of root assets
    std::unordered_set<util::UUID> roots;
    for (const auto& asset : assets) {
        for (const auto& dep : asset.second.dependencies) { roots.erase(dep.uuid); }
        roots.emplace(asset.second.getUUID());
    }

    // second pass to eliminate middle of the tree assets
    for (auto possibleRoot : roots) {
        const auto it = assets.find(possibleRoot);
        for (const auto& dep : it->second.dependencies) { roots.erase(dep.uuid); }
    }

    constexpr std::size_t BundleSoftSize = 10485760; // 10M
    bdl::Manifest manifest;
    std::unordered_set<util::UUID> bundled;
    std::queue<util::UUID> toVisit;
    for (auto root : roots) { toVisit.emplace(root); }

    const auto addToBundle = [this, &manifest](bdl::BundleData& bundle, Asset& asset) -> bool {
        WriteContext context(*this, asset, bundle);
        detail::DriverBase* driver = getDriverLocked(asset.getType());
        if (!driver) {
            BL_LOG_ERROR << "Failed to find driver for asset with type " << asset.getType()
                         << " while adding to bundle";
            return false;
        }
        if (!driver->write(context)) { return false; }
        manifest.assetToBundle[asset.getUUID()] = bundle.uuid;
        return true;
    };

    // add root trees to export based on affinity
    bdl::BundleData currentBundle;
    while (!toVisit.empty()) {
        util::UUID uuid = toVisit.front();
        toVisit.pop();

        Asset& asset               = assets[uuid];
        detail::DriverBase* driver = getDriverLocked(asset.getType());
        if (!driver) {
            BL_LOG_ERROR << "Failed to find driver for asset with type " << asset.getType()
                         << " while adding to bundle";
            continue;
        }
        if (driver->getBundleConfig().affinity != bdl::AssetBundleConfig::Affinity::Parent) {
            continue;
        }

        bundled.emplace(uuid);

        // check if we should flush if next asset is a root
        if (roots.find(asset.getUUID()) != roots.end()) {
            if (currentBundle.data.size() >= BundleSoftSize) {
                if (!currentBundle.flush(path)) {
                    BL_LOG_ERROR << "Failed to flush bundle";
                    return false;
                }
                currentBundle.reset();
            }
        }

        if (!addToBundle(currentBundle, asset)) {
            BL_LOG_ERROR << "Failed to bundle asset " << asset.getUUID();
        }

        for (auto dep : asset.dependencies) {
            if (bundled.find(dep.uuid) != bundled.end()) { continue; }
            toVisit.emplace(dep.uuid);
        }
    }

    // final flush
    if (!currentBundle.data.empty()) {
        if (!currentBundle.flush(path)) {
            BL_LOG_ERROR << "Failed to flush bundle";
            return false;
        }
        currentBundle.reset();
    }

    // bundle remaining assets by type
    std::unordered_map<std::string_view, bdl::BundleData> bundlesByType;
    for (auto& asset : assets) {
        if (bundled.find(asset.second.getUUID()) != bundled.end()) { continue; }
        bundled.emplace(asset.second.getUUID());

        detail::DriverBase* driver = getDriverLocked(asset.second.getType());
        if (!driver) {
            BL_LOG_ERROR << "Failed to find driver for asset with type " << asset.second.getType()
                         << " while adding to bundle";
            continue;
        }
        bdl::BundleData& bundleData = bundlesByType[driver->getSupportedType()];
        if (!addToBundle(bundleData, asset.second)) {
            BL_LOG_ERROR << "Failed to bundle asset " << asset.second.getUUID();
        }

        // flush if necessary
        if (bundleData.data.size() >= BundleSoftSize) {
            if (!bundleData.flush(path)) {
                BL_LOG_ERROR << "Failed to flush bundle";
                return false;
            }
            bundleData.reset();
        }
    }

    // final flush bundles
    for (auto& bundle : bundlesByType) {
        if (!bundle.second.data.empty()) {
            if (!bundle.second.flush(path)) {
                BL_LOG_ERROR << "Failed to flush bundle";
                return false;
            }
            bundle.second.reset();
        }
    }

    // write manifest & metadata
    GameRuntimeData manifestData{
        .assets = &assets, .keyToAsset = &keyToAsset, .bundleManifest = &manifest};
    stream::OutputStream output(
        util::FileUtil::joinPath(path, std::string(bdl::RuntimePaths::ManifestPath)));
    if (!output.isValid()) {
        BL_LOG_ERROR << "Failed to open manifest file for writing";
        return false;
    }
    return serial::binary::Serializer<GameRuntimeData>::serialize(output, manifestData);
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

void Repository::reloadFromSource() {
    std::unique_lock lock(assetMutex);
    for (auto& pair : assets) {
        Asset& asset = pair.second;
        if (!asset.reloadFromSource()) {
            BL_LOG_WARN << "Failed to reload asset with UUID " << pair.first.toString()
                        << " from source";
        }
    }
}

void Repository::forceUnloadAll() {
    std::unique_lock lock(assetMutex);
    for (auto& pair : assets) {
        Asset& asset = pair.second;
        asset.unload(true);
    }
}

} // namespace as
} // namespace bl

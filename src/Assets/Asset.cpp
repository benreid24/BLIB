#include <BLIB/Assets/Asset.hpp>

#include <BLIB/Assets/EditorPaths.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <stdexcept>

namespace bl
{
namespace as
{
Asset::Asset()
: repo(nullptr)
, uuid()
, type()
, metadata()
, payload(nullptr)
, state(State::Unknown)
, refCount(0) {}

Asset::Asset(Repository& r)
: Asset() {
    repo = &r;
}

void Asset::markReadyForAutoSync() { metadata.owner = this; }

bool Asset::initAfterDeserialize(Repository& r) {
    // already inited via dependency chain
    if (state != State::Unknown) { return true; }

    repo  = &r;
    state = State::Unloaded;

    for (const RepoDependency& dep : dependencies) {
        Asset* depAsset = r.getDependencyForInit(dep.uuid);
        if (!depAsset) {
            state = State::Failed;
            BL_LOG_ERROR << "Failed to find dependency with UUID " << dep.uuid.toString()
                         << " for asset " << uuid.toString();
            return false;
        }

        if (!depAsset->initAfterDeserialize(r)) {
            state = State::Failed;
            BL_LOG_ERROR << "Failed to initialize dependency with UUID " << dep.uuid.toString()
                         << " for asset " << uuid.toString();
            return false;
        }
    }

    return true;
}

Payload& Asset::getPayload() {
    if (state != State::Loaded) {
        BL_LOG_ERROR << "Attempted to access asset payload when asset is not loaded";
        throw std::runtime_error("Asset payload is not loaded");
    }
    return *payload;
}

const Payload& Asset::getPayload() const {
    if (state != State::Loaded) {
        BL_LOG_ERROR << "Attempted to access asset payload when asset is not loaded";
        throw std::runtime_error("Asset payload is not loaded");
    }
    return *payload;
}

bool Asset::create(const CreateContext::CreateData& createData) {
    state = State::Unloaded;

    detail::DriverBase* driver = getDriver();
    if (!driver) { return false; }

    state = State::Loading;
    CreateContext context(*repo, *this, createData);
    payload = driver->create(context);
    if (!payload) {
        BL_LOG_ERROR << "Driver failed to create asset of type " << type;
        state = State::Failed;
        return false;
    }
    state = State::Loaded;
    return true;
}

bool Asset::load() {
    if (state == State::Loaded) { return true; }
    if (state == State::Failed) { return false; }

    detail::DriverBase* driver = getDriver();
    if (!driver) { return false; }

    state = State::Loading;
    ReadContext context(*repo, *this);
    payload = driver->read(context);
    if (!payload) {
        BL_LOG_ERROR << "Driver failed to read asset of type " << type;
        state = State::Failed;
        return false;
    }
    state = State::Loaded;
    return true;
}

bool Asset::unload(bool force) {
    if (state != State::Loaded) { return true; }

    if (refCount > 0 && !force) {
        BL_LOG_WARN << "Attempted to unload asset with " << refCount << " outstanding references";
        return false;
    }

    payload.reset();
    state = State::Unloaded;

    return true;
}

bool Asset::saveEditor() {
    // create directories
    const std::string assetDir = EditorPaths::getAssetPath(repo->getAssetDirectory(), *this);
    const std::string filesDir = EditorPaths::getAssetFilesPath(repo->getAssetDirectory(), *this);
    if (!util::FileUtil::createDirectory(assetDir)) {
        BL_LOG_ERROR << "Failed to create asset directory: " << assetDir;
        return false;
    }
    if (!util::FileUtil::createDirectory(filesDir)) {
        BL_LOG_ERROR << "Failed to create asset files directory: " << filesDir;
        return false;
    }

    // write metadata file
    std::string metadataPath =
        EditorPaths::getAssetMetadataFilePath(repo->getAssetDirectory(), *this);
    stream::OutputStream metadataFile(metadataPath);
    if (!serial::json::Serializer<Asset>::serializeStream(metadataFile, *this, 4, 0)) {
        BL_LOG_ERROR << "Failed to write asset metadata to " << metadataPath;
        return false;
    }
    metadataFile.close();

    // write payload if loaded
    if (!writePayload()) { return false; }

    return true;
}

bool Asset::writePayload() {
    if (repo->getMode() == Mode::Editor && payload) {
        WriteContext ctx(*repo, *this);
        detail::DriverBase* driver = getDriver();
        if (!driver) { return false; }
        return driver->write(ctx);
    }
    return true;
}

void Asset::flushPayload() {
    if (!writePayload()) {
        BL_LOG_ERROR << "Failed to flush asset " << uuid.toString() << " to disk";
    }
}

void Asset::handleDisplayNameChange(const std::string& oldName, const std::string& newName) {
    const std::string oldDir =
        EditorPaths::getAssetPath(repo->getAssetDirectory(), metadata.getPath(), oldName);
    const std::string newDir =
        EditorPaths::getAssetPath(repo->getAssetDirectory(), metadata.getPath(), newName);

    if (!util::FileUtil::moveDirectory(oldDir, newDir)) {
        BL_LOG_ERROR << "Failed to move asset directory from " << oldDir << " to " << newDir
                     << " for rename";
    }
}

void Asset::handlePathChange(const std::string& oldPath, const std::string& newPath) {
    const std::string oldDir =
        EditorPaths::getAssetPath(repo->getAssetDirectory(), oldPath, metadata.getDisplayName());
    const std::string newDir =
        EditorPaths::getAssetPath(repo->getAssetDirectory(), newPath, metadata.getDisplayName());

    if (!util::FileUtil::moveDirectory(oldDir, newDir)) {
        BL_LOG_ERROR << "Failed to move asset directory from " << oldDir << " to " << newDir
                     << " for path change";
    }
}

void Asset::handleAutoLoadChange() { repo->writeManifest(); }

detail::DriverBase* Asset::getDriver() {
    detail::DriverBase* driver = repo->getDriver(type);
    if (!driver) {
        BL_LOG_ERROR << "Attempted to load asset with type " << type
                     << " but no driver is registered for that type";
        state = State::Failed;
        return nullptr;
    }
    return driver;
}

} // namespace as
} // namespace bl

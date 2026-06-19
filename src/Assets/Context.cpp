#include <BLIB/Assets/Context.hpp>

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/EditorPaths.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <fstream>

namespace bl
{
namespace as
{
namespace
{
std::string EmptyString;
}
namespace detail
{
Context::Context(Repository& repo, Asset& asset)
: repo(repo)
, asset(asset) {}

Mode Context::getMode() const { return repo.getMode(); }

std::string Context::getFilesDirectory() const {
    if (getMode() == Mode::Game) {
        BL_LOG_WARN << "Asset repo paths are not valid while in game mode";
    }
    return EditorPaths::getAssetFilesPath(repo.getAssetDirectory(), asset);
}

std::string Context::getFilePath(std::string_view filename) const {
    if (getMode() == Mode::Game) {
        BL_LOG_WARN << "Asset repo paths are not valid while in game mode";
    }

    return util::FileUtil::joinPath(getFilesDirectory(), std::string(filename));
}

PersistentStream* Context::getPersistentStream(std::string_view filename) const {
    return repo.getPersistentStream(asset, filename);
}

bool Context::setupReadStream(std::string_view filename, stream::InputStream& input) const {
    if (getMode() == Mode::Game) { BL_LOG_WARN << "Creating read stream while in game mode"; }

    // always read from repo in base
    return input.open(getFilePath(filename));
}

bool Context::setupWriteStream(std::string_view filename, stream::OutputStream& output) const {
    if (getMode() == Mode::Game) { BL_LOG_WARN << "Creating write stream while in game mode"; }

    // always write to repo in base
    return output.open(getFilePath(filename));
}

} // namespace detail

CreateContext::CreateData::CreateData()
: CreateData(EmptyString) {}

CreateContext::CreateContext(Repository& repo, Asset& asset, const CreateData& data)
: Context(repo, asset)
, customData(data) {}

ReadContext::ReadContext(Repository& repo, bdl::BundleRuntime& br, Asset& asset)
: Context(repo, asset)
, bundleRuntime(br) {}

bool ReadContext::setupReadStream(std::string_view filename, stream::InputStream& input) const {
    switch (getMode()) {
    case Mode::Game:
        return bundleRuntime.initStream(input, asset.getUUID(), filename);
    case Mode::Editor:
        return Context::setupReadStream(filename, input);

    default:
        BL_LOG_ERROR << "Invalid asset context mode " << static_cast<int>(getMode());
        return false;
    }
}

WriteContext::WriteContext(Repository& repo, Asset& asset)
: Context(repo, asset)
, priorFileOffset(0)
, priorFile()
, bundle(nullptr) {}

WriteContext::WriteContext(Repository& repo, Asset& asset, bdl::BundleData& b)
: WriteContext(repo, asset) {
    bundle = &b;
}

WriteContext::~WriteContext() {
    if (getMode() == Mode::BundleCreation) { flushBundleWrite(); }
}

Mode WriteContext::getMode() const {
    if (bundle) { return Mode::BundleCreation; }
    return Context::getMode();
}

bool WriteContext::setupWriteStream(std::string_view filename, stream::OutputStream& output) {
    switch (getMode()) {
    case Mode::Game: // equal to BundleCreation
        if (!bundle) {
            BL_LOG_ERROR << "Writing in Game mode but bundle not present";
            return false;
        }
        flushBundleWrite();
        priorFile       = filename;
        priorFileOffset = bundle->data.size();
        output.open(bundle->data);
        return true;
    case Mode::Editor:
        return Context::setupWriteStream(filename, output);
    default:
        BL_LOG_ERROR << "Invalid asset context mode " << static_cast<int>(getMode());
        return false;
    }
}

void WriteContext::flushBundleWrite() {
    if (!priorFile.empty() && bundle) {
        const std::size_t end  = bundle->data.size();
        const std::size_t size = end > priorFileOffset ? end - priorFileOffset : 0;
        auto& fileMap          = bundle->assetFileManifest[asset.getUUID()];
        std::string path(priorFile);
        auto result = fileMap.try_emplace(path, path, priorFileOffset, size);
        if (!result.second) {
            BL_LOG_ERROR << "Failed to save duplicate file '" << path << "' for asset "
                         << asset.getUUID() << " to bundle";
        }

        // clear so we do not double write in destructor
        priorFile = std::string_view();
    }
}

} // namespace as
} // namespace bl

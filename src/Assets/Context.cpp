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
    return repo.getPersistentStream(asset.getUUID(), getFilePath(filename));
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

CreateContext::CreateContext(Repository& repo, Asset& asset, const CreateData& data)
: Context(repo, asset)
, customData(data) {}

ReadContext::ReadContext(Repository& repo, Asset& asset)
: Context(repo, asset) {}

bool ReadContext::setupReadStream(std::string_view filename, stream::InputStream& input) const {
    switch (getMode()) {
    case Mode::Game:
        // TODO - implement bundle reading
        return false;
    case Mode::Editor:
        return Context::setupReadStream(filename, input);

    default:
        BL_LOG_ERROR << "Invalid asset context mode " << static_cast<int>(getMode());
        return false;
    }
}

WriteContext::WriteContext(Repository& repo, Asset& asset)
: Context(repo, asset) {}

bool WriteContext::setupWriteStream(std::string_view filename, stream::OutputStream& output) const {
    switch (getMode()) {
    case Mode::Game:
        // TODO - implement bundle writing
        return false;
    case Mode::Editor:
        return Context::setupWriteStream(filename, output);
    default:
        BL_LOG_ERROR << "Invalid asset context mode " << static_cast<int>(getMode());
        return false;
    }
}

} // namespace as
} // namespace bl

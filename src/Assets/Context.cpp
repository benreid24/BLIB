#include <BLIB/Assets/Context.hpp>

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/EditorPaths.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <fstream>

namespace bl
{
namespace as
{
namespace detail
{
Context::Context(Mode mode, Repository& repo, Asset& asset)
: mode(mode)
, repo(repo)
, asset(asset) {}

std::string Context::getFilePath(std::string_view filename) const {
    return util::FileUtil::joinPath(
        EditorPaths::getAssetFilesPath(asset.getType(), asset.getMetadata().getDisplayName()),
        std::string(filename));
}

} // namespace detail

CreateContext::CreateContext(Mode mode, Repository& repo, Asset& asset, std::string_view path)
: Context(mode, repo, asset)
, path(path) {}

ReadContext::ReadContext(Mode mode, Repository& repo, Asset& asset)
: Context(mode, repo, asset) {}

bool ReadContext::readFile(std::string_view filename, std::vector<char>& outBuffer) const {
    switch (getMode()) {
    case Mode::Game:
        // TODO - implement bundle reading
        return false;
    case Mode::Editor:
        return util::FileUtil::readFile(getFilePath(filename), outBuffer);

    default:
        BL_LOG_ERROR << "Invalid asset context mode " << static_cast<int>(getMode());
        return false;
    }
}

WriteContext::WriteContext(Mode mode, Repository& repo, Asset& asset)
: Context(mode, repo, asset) {}

bool WriteContext::writeFile(std::string_view filename, const std::vector<char>& buffer) const {
    switch (getMode()) {
    case Mode::Game:
        // TODO - implement bundle writing
        return false;
    case Mode::Editor: {
        std::ofstream out(getFilePath(filename), std::ios::binary);
        if (!out) {
            BL_LOG_ERROR << "Failed to open file for writing: " << getFilePath(filename);
            return false;
        }
        out.write(buffer.data(), buffer.size());
        if (!out) {
            BL_LOG_ERROR << "Failed to write data to file: " << getFilePath(filename);
            return false;
        }
        return true;
    }
    default:
        BL_LOG_ERROR << "Invalid asset context mode " << static_cast<int>(getMode());
        return false;
    }
}

} // namespace as
} // namespace bl

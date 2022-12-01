#include <BLIB/Logging.hpp>
#include <BLIB/Resources/Bundling/Bundle.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
namespace
{}

Bundle::Bundle(const std::string& path)
: source(path)
, touchTime(std::chrono::steady_clock::now()) {
    fileHandle.open(path.c_str(), std::ios::binary);
    if (fileHandle.good()) {
        if (manifest.load(fileHandle)) { BL_LOG_INFO << "Loaded bundle '" << path << "'"; }
        else {
            BL_LOG_ERROR << "Failed to load bundle '" << path << "'";
        }
    }
    else {
        BL_LOG_ERROR << "Failed to open bundle file: '" << path << "'";
    }
}

bool Bundle::getResource(const std::string& path, std::vector<char>& buffer) {
    touchTime = std::chrono::steady_clock::now();

    const BundledFileMetadata info = manifest.getFileInfo(path);
    if (info.offset == 0) {
        BL_LOG_ERROR << "Resource '" << path << "' not found in bundle '" << source << "'";
        return false;
    }

    fileHandle.seekg(info.offset);
    if (!fileHandle.good()) {
        BL_LOG_ERROR << "Resource '" << path << "' has invalid offset " << info.offset
                     << " in bundle '" << source << "'";
        return false;
    }
    buffer.resize(info.length);
    return fileHandle.read(buffer.data(), info.length).good();
}

bool Bundle::expired() const {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() -
                                                            touchTime)
               .count() >= Lifetime;
}

} // namespace bundle
} // namespace resource
} // namespace bl

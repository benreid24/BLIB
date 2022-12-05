#include <BLIB/Resources/Bundling/Bundle.hpp>

#include "Utils.hpp"
#include <BLIB/Logging.hpp>

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
    std::ifstream fileHandle(path.c_str(), std::ios::binary);
    if (fileHandle.good()) {
        if (manifest.load(fileHandle)) {
            const std::streampos cpos = fileHandle.tellg();
            fileHandle.seekg(0, std::ios_base::end);
            const std::streamoff dataSize = fileHandle.tellg() - cpos;
            fileHandle.seekg(cpos);
            data.resize(dataSize);
            fileHandle.read(data.data(), dataSize);
            BL_LOG_INFO << "Loaded bundle '" << path << "' size=" << formatSize(data.size());
        }
        else { BL_LOG_ERROR << "Failed to load bundle '" << path << "'"; }
    }
    else { BL_LOG_ERROR << "Failed to open bundle file: '" << path << "'"; }
}

bool Bundle::getResource(const std::string& path, char** buf, std::size_t& len) {
    touchTime = std::chrono::steady_clock::now();

    const BundledFileMetadata info = manifest.getFileInfo(path);
    if (info.length < 0) {
        BL_LOG_ERROR << "Resource '" << path << "' not found in bundle '" << source << "'";
        return false;
    }
    if (info.offset >= static_cast<std::int64_t>(data.size())) {
        BL_LOG_ERROR << "Resource '" << path << "' has invalid offset " << info.offset
                     << " in bundle '" << source << "'";
        return false;
    }
    if (info.offset + info.length >= static_cast<std::int64_t>(data.size())) {
        BL_LOG_ERROR << "Resource '" << path << "' has invalid offset + size (" << info.offset
                     << " + " << info.length << ") in bundle '" << source << "'";
        return false;
    }
    *buf = &data[info.offset];
    len  = info.length;
    return true;
}

bool Bundle::expired() const {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() -
                                                            touchTime)
               .count() >= Lifetime;
}

} // namespace bundle
} // namespace resource
} // namespace bl

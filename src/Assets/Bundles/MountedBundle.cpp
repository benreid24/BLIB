#include <BLIB/Assets/Bundles/MountedBundle.hpp>

namespace bl
{
namespace as
{
namespace bdl
{
MountedBundle::MountedBundle(Repository& owner, const std::string& path)
: bundlePath(path)
, touchTime(std::chrono::steady_clock::now()) {
    stream::InputStream input;
    if (!input.open(path)) {
        BL_LOG_ERROR << "Failed to mount bundle: " << path;
        return;
    }

    if (!serial::binary::Serializer<BundleData>::deserialize(input, data)) {
        BL_LOG_ERROR << "Failed to load bundle: " << path;
    }
}

bool MountedBundle::initStream(stream::InputStream& stream, util::UUID uuid,
                               std::string_view path) {
    touchTime = std::chrono::steady_clock::now();

    const FileMetadata* md = findFile(uuid, path);
    if (!md) { return false; }

    stream.open(std::span<char>(&data.data[md->offset], md->size));
    return true;
}

bool MountedBundle::initStreamDirect(stream::InputStream& stream, util::UUID uuid,
                                     std::string_view path) {
    touchTime = std::chrono::steady_clock::now();

    const FileMetadata* md = findFile(uuid, path);
    if (!md) { return false; }

    return stream.open(bundlePath, data.headerSize + md->offset, md->size);
}

const FileMetadata* MountedBundle::findFile(util::UUID uuid, std::string_view path) {
    const auto assetIt = data.assetFileManifest.find(uuid);
    if (assetIt == data.assetFileManifest.end()) { return nullptr; }

    const auto fileIt = assetIt->second.find(std::string(path));
    if (fileIt == assetIt->second.end()) { return nullptr; }

    return &fileIt->second;
}

} // namespace bdl
} // namespace as
} // namespace bl

#include <BLIB/Assets/EditorPaths.hpp>

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Util/FileUtil.hpp>

namespace bl
{
namespace as
{
namespace
{
std::string filterAssetName(const std::string& name) {
    std::string filtered;
    filtered.reserve(name.size());
    for (char c : name) {
        if (c == '/' || c == '\\' || c == '.') { filtered += '_'; }
        else { filtered += c; }
    }
    return filtered;
}
} // namespace

std::string EditorPaths::getAssetFolderName(util::UUID uuid, const std::string& displayName) {
    return displayName + "_" + uuid.toString();
}

std::optional<std::pair<util::UUID, std::string>> EditorPaths::parseAssetFolderName(
    const std::string& folderName) {
    const std::size_t i = folderName.find_last_of('_');
    if (i == std::string::npos) { return std::nullopt; }

    const std::string displayName = folderName.substr(0, i);
    const std::string uuidStr     = folderName.substr(i + 1);

    util::UUID uuid;
    if (!uuid.parse(uuidStr)) { return std::nullopt; }

    return std::make_pair(uuid, displayName);
}

std::string EditorPaths::getAssetPath(const std::string& repoRoot, const Asset& asset) {
    return getAssetPath(repoRoot,
                        asset.getMetadata().getPath(),
                        asset.getUUID(),
                        asset.getMetadata().getDisplayName());
}

std::string EditorPaths::getAssetPath(const std::string& repoRoot, const std::string& assetFolder,
                                      util::UUID uuid, const std::string& assetName) {
    const std::string assetPath = util::FileUtil::joinPath(repoRoot, assetFolder);
    return util::FileUtil::joinPath(assetPath,
                                    getAssetFolderName(uuid, filterAssetName(assetName)));
}

std::string EditorPaths::getAssetFilesPath(const std::string& repoRoot, const Asset& asset) {
    const std::string assetPath = getAssetPath(repoRoot, asset);
    return util::FileUtil::joinPath(assetPath, std::string(FilesDirname));
}

std::string EditorPaths::getAssetFilesPath(const std::string& assetPath) {
    return util::FileUtil::joinPath(assetPath, std::string(FilesDirname));
}

std::string EditorPaths::getAssetMetadataFilePath(const std::string& repoRoot, const Asset& asset) {
    const std::string assetPath = getAssetPath(repoRoot, asset);
    return util::FileUtil::joinPath(assetPath,
                                    asset.getUUID().toString() + std::string(MetadataExtension));
}

} // namespace as
} // namespace bl

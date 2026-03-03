#include <BLIB/Assets/EditorPaths.hpp>

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Util/FileUtil.hpp>

namespace bl
{
namespace as
{
std::string EditorPaths::getAssetPath(const std::string& repoRoot, const Asset& asset) {
    const std::string assetRoot = util::FileUtil::joinPath(repoRoot, std::string(RootPath));
    const std::string assetPath =
        util::FileUtil::joinPath(assetRoot, asset.getMetadata().getPath());
    return util::FileUtil::joinPath(assetPath, asset.getMetadata().getDisplayName());
}

std::string EditorPaths::getAssetFilesPath(const std::string& repoRoot, const Asset& asset) {
    const std::string assetPath = getAssetPath(repoRoot, asset);
    return util::FileUtil::joinPath(assetPath, std::string(FilesDirname));
}

std::string EditorPaths::getAssetMetadataFilePath(const std::string& repoRoot, const Asset& asset) {
    const std::string assetPath = getAssetPath(repoRoot, asset);
    return util::FileUtil::joinPath(assetPath,
                                    asset.getUUID().toString() + std::string(MetadataExtension));
}

} // namespace as
} // namespace bl

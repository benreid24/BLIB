#include <BLIB/Assets/EditorPaths.hpp>

#include <BLIB/Util/FileUtil.hpp>

namespace bl
{
namespace as
{
std::string EditorPaths::getAssetPath(std::string_view assetType, std::string_view assetDiskName) {
    const std::string typePath =
        util::FileUtil::joinPath(std::string(RootPath), std::string(assetType));
    return util::FileUtil::joinPath(typePath, std::string(assetDiskName));
}

std::string EditorPaths::getAssetFilesPath(std::string_view assetType,
                                           std::string_view assetDiskName) {
    const std::string assetPath = getAssetPath(assetType, assetDiskName);
    return util::FileUtil::joinPath(assetPath, std::string(FilesDirname));
}

std::string EditorPaths::getAssetMetadataFilePath(std::string_view assetType,
                                                  std::string_view assetDiskName,
                                                  const std::string& assetUuid) {
    const std::string assetPath = getAssetPath(assetType, assetDiskName);
    return util::FileUtil::joinPath(assetPath, assetUuid + std::string(MetadataExtension));
}

} // namespace as
} // namespace bl

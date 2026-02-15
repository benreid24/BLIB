#ifndef BLIB_ASSETS_EDITORPATHS_HPP
#define BLIB_ASSETS_EDITORPATHS_HPP

#include <string>
#include <string_view>

namespace bl
{
namespace as
{
/**
 * @brief Collection of helpers for managing assets on disk in editor format
 *
 * @ingroup Assets
 */
struct EditorPaths {
    /// The root path for all assets on disk
    static constexpr std::string_view RootPath = "Assets";

    /// The directory in the asset folder where asset files are stored
    static constexpr std::string_view FilesDirname = "files";

    /// File extension of asset metadata files
    static constexpr std::string_view MetadataExtension = ".asset";

    /**
     * @brief Returns the path to the asset on disk for the given type and asset name
     *
     * @param assetType The asset type id
     * @param assetDiskName The asset display name
     * @return The path to the asset on disk for the given type and asset name
     */
    static std::string getAssetPath(std::string_view assetType, std::string_view assetDiskName);

    /**
     * @brief Returns the path to the directory containing the asset files on disk
     *
     * @param assetType The asset type id
     * @param assetDiskName The asset display name
     * @return The path to where asset files should be stored
     */
    static std::string getAssetFilesPath(std::string_view assetType,
                                         std::string_view assetDiskName);

    /**
     * @brief Returns the path to the asset metadata file for the given asset
     *
     * @param assetType The asset type id
     * @param assetDiskName The asset display name
     * @param assetUuid The asset uuid
     * @return The path to the asset metadata file for the given asset
     */
    static std::string getAssetMetadataFilePath(std::string_view assetType,
                                                std::string_view assetDiskName,
                                                const std::string& assetUuid);
};

} // namespace as
} // namespace bl

#endif

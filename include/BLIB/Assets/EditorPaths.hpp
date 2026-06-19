#ifndef BLIB_ASSETS_EDITORPATHS_HPP
#define BLIB_ASSETS_EDITORPATHS_HPP

#include <BLIB/Util/UUID.hpp>
#include <string>
#include <string_view>

namespace bl
{
namespace as
{
class Asset;

/**
 * @brief Collection of helpers for managing assets on disk in editor format
 *
 * @ingroup Assets
 */
struct EditorPaths {
    /// The directory in the asset folder where asset files are stored
    static constexpr std::string_view FilesDirname = "files";

    /// File extension of asset metadata files
    static constexpr std::string_view MetadataExtension = ".asset";

    /**
     * @brief Creates the base folder name for an asset
     *
     * @param uuid The UUID of the asset
     * @param displayName The display name of the asset
     * @return The folder name. In form "{DisplayName}_{UUID}"
     */
    static std::string getAssetFolderName(util::UUID uuid, const std::string& displayName);

    /**
     * @brief Parses a foldername into a UUID and asset display name
     *
     * @param folderName The name of the folder to parse
     * @return The parsed parts or std::nullopt if parsing fails
     */
    static std::optional<std::pair<util::UUID, std::string>> parseAssetFolderName(
        const std::string& folderName);

    /**
     * @brief Returns the path to the asset on disk
     *
     * @param repoRoot The root path of the asset repository
     * @param asset The asset to get the path for
     * @return The path to the asset on disk
     */
    static std::string getAssetPath(const std::string& repoRoot, const Asset& asset);

    /**
     * @brief Returns the path to the asset on disk
     *
     * @param repoRoot The root path of the asset repository
     * @param assetFolder The folder path of the asset
     * @param uuid The UUID of the asset
     * @param assetName The display name of the asset
     * @return The path to the asset on disk
     */
    static std::string getAssetPath(const std::string& repoRoot, const std::string& assetFolder,
                                    util::UUID uuid, const std::string& assetName);

    /**
     * @brief Returns the path to the directory containing the asset files on disk
     *
     * @param repoRoot The root path of the asset repository
     * @param asset The asset to get the path for
     * @return The path to where asset files should be stored
     */
    static std::string getAssetFilesPath(const std::string& repoRoot, const Asset& asset);

    /**
     * @brief Returns the path to the directory containing the asset files on disk
     *
     * @param assetPath The directory of the asset
     * @return The directory to put asset payload files in
     */
    static std::string getAssetFilesPath(const std::string& assetPath);

    /**
     * @brief Returns the path to the asset metadata file for the given asset
     *
     * @param repoRoot The root path of the asset repository
     * @param asset The asset to get the path for
     * @return The path to the asset metadata file for the given asset
     */
    static std::string getAssetMetadataFilePath(const std::string& repoRoot, const Asset& asset);
};

} // namespace as
} // namespace bl

#endif

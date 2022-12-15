#ifndef BLIB_RESOURCES_BUNDLING_CONFIG_HPP
#define BLIB_RESOURCES_BUNDLING_CONFIG_HPP

#include <BLIB/Resources/Bundling/BundleSource.hpp>
#include <BLIB/Resources/Bundling/FileHandler.hpp>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * @addtogroup Bundling
 * @ingroup Resources
 * @brief Resource bundling utility for combining external files into bundles
 *
 */

namespace bl
{
namespace resource
{
/// @brief Resource bundling utility for combining external files into bundles
namespace bundle
{
class BundleCreator;

/**
 * @brief Bundler configuration class. This determines the behavior of the bundler
 *
 * @ingroup Bundling
 *
 */
class Config {
public:
    /**
     * @brief Construct a new Config with the given output directory
     *
     * @param outputPath Directory to output the manifest and bundles to
     */
    Config(const std::string& outputPath);

    /**
     * @brief Sets the top-level directory to find and bundle all files not explicitly captured by
     *        one of the bundle sources
     *
     * @param path The catch-all directory
     * @return Config& A reference to this object
     */
    Config& withCatchAllDirectory(const std::string& path);

    /**
     * @brief Adds an explicit bundle source to the config
     *
     * @param source The source of a bundle to create
     * @return Config& A reference to this object
     */
    Config& addBundleSource(BundleSource&& source);

    /**
     * @brief Adds a regex pattern to exclude files from directory listings. Files explicitly
     *        specified in sources or as dependencies are never excluded
     *
     * @param pattern The regex pattern to exclude based on
     * @return Config& A reference to this object
     */
    Config& addExcludePattern(const std::string& pattern);

    /**
     * @brief Adds a file path to be specifically excluded from bundling
     *
     * @param path The file to exclude
     * @return Config& A reference to this object
     */
    Config& addExcludeFile(const std::string& path);

    /**
     * @brief Adds a file handler to the config
     *
     * @tparam THandler The type of handler to create and add
     * @tparam TArgs Arguments to the handler's constructor
     * @param pattern File pattern that, when matched, causes a file to use this handler
     * @param args Arguments to the handler's constructor
     * @return Config& A reference to this object
     */
    template<typename THandler, typename... TArgs>
    Config& addFileHandler(const std::string& pattern, TArgs&&... args);

    /**
     * @brief Convenience method to get the rvalue ref required by the bundler
     *
     * @return Config&& A reference to this object
     */
    constexpr inline Config&& build();

    /**
     * @brief Returns the directory where bundles are created
     *
     */
    const std::string& outputDirectory() const;

    /**
     * @brief Returns the top-level directory to bundle all remaining files from
     *
     */
    const std::string& catchAllDirectory() const;

    /**
     * @brief Returns all of the bundle sources in the config
     *
     */
    const std::vector<BundleSource>& bundleSources() const;

    /**
     * @brief Checks whether or not the given file should be bundled or not
     *
     * @param path The file path to check
     * @return True if the file should be bundled, false if not
     */
    bool includeFile(const std::string& path) const;

    /**
     * @brief Returns the file handler to use for the given file. Files not matching any handler
     *        will use DefaultFileHandler
     *
     * @param path The file path to get the handler for
     * @return FileHandler& File handler to process the file with
     */
    FileHandler& getFileHandler(const std::string& path) const;

private:
    const std::string outDir;
    std::string allFilesDir;
    std::vector<BundleSource> sources;
    std::vector<std::pair<std::string, std::regex>> excludePatterns;
    std::unordered_set<std::string> excludeFiles;
    std::vector<std::pair<std::regex, std::unique_ptr<FileHandler>>> handlers;

    friend class BundleCreator;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename THandler, typename... TArgs>
Config& Config::addFileHandler(const std::string& pattern, TArgs&&... args) {
    handlers.emplace_back(pattern.c_str(),
                          std::make_unique<THandler>(std::forward<TArgs>(args)...));
    return *this;
}

constexpr inline Config&& Config::build() { return std::move(*this); }

} // namespace bundle
} // namespace resource
} // namespace bl

#endif

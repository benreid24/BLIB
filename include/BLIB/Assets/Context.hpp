#ifndef BLIB_ASSETS_CONTEXT_HPP
#define BLIB_ASSETS_CONTEXT_HPP

#include <BLIB/Assets/Mode.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace bl
{
namespace as
{
class Asset;
class Repository;

namespace detail
{
/**
 * @brief Context for asset loading. This is passed to the loader and can be used to provide
 *        information about the load and helper methods for loading nested files and such.
 *
 * @ingroup Assets
 */
class Context {
public:
    /**
     * @brief Creates a context for asset loading
     *
     * @param mode The mode the asset system is in
     * @param repo
     * @param asset The asset being loaded
     */
    Context(Mode mode, Repository& repo, Asset& asset);

    /**
     * @brief Returns the mode the asset system is in
     */
    Mode getMode() const { return mode; }

    /**
     * @brief Returns the repository the asset belongs to
     */
    Repository& getRepository() const { return repo; }

    /**
     * @brief Returns the asset being loaded
     */
    Asset& getAsset() const { return asset; }

protected:
    /**
     * @brief Returns the full path for the given filename relative to the asset's files directory
     *
     * @param filename The filename to use
     * @return The full path to use
     */
    std::string getFilePath(std::string_view filename) const;

private:
    Mode mode;
    Repository& repo;
    Asset& asset;
};

} // namespace detail

/**
 * @brief Context for asset imports. Import is when assets are created from external files
 *
 * @ingroup Assets
 */
class CreateContext : public detail::Context {
public:
    /**
     * @brief Creates a context for asset importing
     *
     * @param mode The mode the asset system is in
     * @param repo The repository the asset belongs to
     * @param asset The asset being imported
     * @param path The path to the source file being imported from
     */
    CreateContext(Mode mode, Repository& repo, Asset& asset, std::string_view path);

    /**
     * @brief Returns the path of the external file or folder being imported from
     */
    std::string_view getSourcePath() const { return path; }

private:
    std::string_view path;
};

/**
 * @brief Context for existing assets being loaded into memory
 *
 * @ingroup Assets
 */
class ReadContext : public detail::Context {
public:
    /**
     * @brief Creates the context for asset loading
     *
     * @param mode The mode the asset system is in
     * @param repo The repository the asset belongs to
     * @param asset The asset being loaded
     */
    ReadContext(Mode mode, Repository& repo, Asset& asset);

    /**
     * @brief Reads the contents of the given asset file for the current asset
     *
     * @param filename The local name of the file to read
     * @param outBuffer The buffer to write into
     * @return True if the file was able to be read, false otherwise
     */
    bool readFile(std::string_view filename, std::vector<char>& outBuffer) const;

    // TODO - stream based methods?

private:
    //
};

/**
 * @brief Context for writing asset data to disk
 *
 * @ingroup Assets
 */
class WriteContext : public detail::Context {
public:
    /**
     * @brief Creates the write context
     *
     * @param mode The mode the asset system is in
     * @param repo The repository the asset belongs to
     * @param asset The asset being written
     */
    WriteContext(Mode mode, Repository& repo, Asset& asset);

    /**
     * @brief Writes the file to asset storage
     *
     * @param filename The filename to use
     * @param buffer The content of the file to write
     * @return True if the file could be written, false otherwise
     */
    bool writeFile(std::string_view filename, const std::vector<char>& buffer) const;

    // TODO - stream based methods?

private:
    //
};

} // namespace as
} // namespace bl

#endif

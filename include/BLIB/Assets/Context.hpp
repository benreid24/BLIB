#ifndef BLIB_ASSETS_CONTEXT_HPP
#define BLIB_ASSETS_CONTEXT_HPP

#include <BLIB/Assets/Mode.hpp>
#include <BLIB/Assets/PersistentStream.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Streams.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace bl
{
namespace as
{
class Asset;
class Repository;

namespace bdl
{
class BundleRuntime;
}

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
    Context(Repository& repo, Asset& asset);

    /**
     * @brief Returns the mode the asset system is in
     */
    Mode getMode() const;

    /**
     * @brief Returns the repository the asset belongs to
     */
    Repository& getRepository() const { return repo; }

    /**
     * @brief Returns the asset being loaded
     */
    Asset& getAsset() const { return asset; }

    /**
     * @brief Returns an input stream to the given file. The stream will remain valid until the
     *        asset is unloaded. Use this method for payloads that continue to stream from disk
     *        after initialization, such as sf::Music
     *
     * @param filename The local name of the file to get a stream for
     * @return A pointer to the stream, or nullptr if the stream could not be created
     */
    PersistentStream* getPersistentStream(std::string_view filename) const;

    /**
     * @brief Reads the contents of the given asset file for the current asset
     *
     * @param filename The local name of the file to read
     * @param input The stream to populate with the file contents
     * @return True if the file was able to be read, false otherwise
     */
    bool setupReadStream(std::string_view filename, stream::InputStream& input) const;

    /**
     * @brief Writes the file to asset storage
     *
     * @param filename The filename to use
     * @param output The stream to setup for output
     * @return True if the file can be written, false otherwise
     */
    bool setupWriteStream(std::string_view filename, stream::OutputStream& output) const;

    /**
     * @brief Returns the directory that asset files are stored in
     */
    std::string getFilesDirectory() const;

    /**
     * @brief Returns the full path for the given filename relative to the asset's files directory
     *
     * @param filename The filename to use
     * @return The full path to use
     */
    std::string getFilePath(std::string_view filename) const;

protected:
    Repository& repo;
    Asset& asset;
};

} // namespace detail

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
     * @param repo The repository the asset belongs to
     * @param bundleRuntime The repository bundle runtime
     * @param asset The asset being loaded
     */
    ReadContext(Repository& repo, bdl::BundleRuntime& bundleRuntime, Asset& asset);

    /**
     * @brief Reads the contents of the given asset file for the current asset
     *
     * @param filename The local name of the file to read
     * @param input The stream to populate with the file contents
     * @return True if the file was able to be read, false otherwise
     */
    bool setupReadStream(std::string_view filename, stream::InputStream& input) const;

private:
    bdl::BundleRuntime& bundleRuntime;
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
    WriteContext(Repository& repo, Asset& asset);

    /**
     * @brief Writes the file to asset storage
     *
     * @param filename The filename to use
     * @param output The stream to setup for output
     * @return True if the file can be written, false otherwise
     */
    bool setupWriteStream(std::string_view filename, stream::OutputStream& output) const;
};

/**
 * @brief Context for asset imports. Import is when assets are created from external files
 *
 * @ingroup Assets
 */
class CreateContext : public detail::Context {
public:
    /// Custom data can be passed to asset drivers by deriving from this struct
    class CreateData {
    public:
        /**
         * @brief Creates empty create data
         */
        CreateData();

        /**
         * @brief Creates the custom data. Path is provided for convenience
         *
         * @param path The path to create from
         */
        CreateData(const std::string& path)
        : path(&path) {}

        /**
         * @brief Destroys the custom data
         */
        virtual ~CreateData() = default;

        /**
         * @brief Returns the file path that should be imported from. Not a required field
         */
        const std::string& getPath() const { return *path; }

    private:
        const std::string* path;
    };

    /**
     * @brief Creates a context for asset importing
     *
     * @param mode The mode the asset system is in
     * @param repo The repository the asset belongs to
     * @param asset The asset being imported
     * @param customData Custom data to pass to the driver for asset creation
     */
    CreateContext(Repository& repo, Asset& asset, const CreateData& customData);

    /**
     * @brief Returns the custom data passed to the driver for asset creation
     */
    const CreateData& getCustomData() const { return customData; }

    /**
     * @brief Returns the custom data cast as the given type
     *
     * @tparam T The type to cast to
     * @return The custom data cast as the given type
     */
    template<typename T>
    const T& getCustomDataAs() const {
        const T* cast = dynamic_cast<const T*>(&customData);
        if (!cast) {
            BL_LOG_ERROR << "Invalid custom data cast to " << typeid(T).name();
            throw std::bad_cast();
        }
        return *cast;
    }

    /**
     * @brief Returns the custom data cast as the given type
     *
     * @tparam T The type to cast to
     * @return The custom data cast as the given type
     */
    template<typename T>
    const T* getCustomDataAsMaybe() const {
        const T* cast = dynamic_cast<const T*>(&customData);
        if (!cast) { return nullptr; }
        return cast;
    }

private:
    const CreateData& customData;
};

} // namespace as
} // namespace bl

#endif

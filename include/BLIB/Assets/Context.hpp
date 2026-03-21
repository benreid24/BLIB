#ifndef BLIB_ASSETS_CONTEXT_HPP
#define BLIB_ASSETS_CONTEXT_HPP

#include <BLIB/Assets/Mode.hpp>
#include <BLIB/Logging.hpp>
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

protected:
    /**
     * @brief Returns the full path for the given filename relative to the asset's files directory
     *
     * @param filename The filename to use
     * @return The full path to use
     */
    std::string getFilePath(std::string_view filename) const;

private:
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
    /// Custom data can be passed to asset drivers by deriving from this struct
    struct CustomData {
        virtual ~CustomData() = default;
    };

    /**
     * @brief Creates a context for asset importing
     *
     * @param mode The mode the asset system is in
     * @param repo The repository the asset belongs to
     * @param asset The asset being imported
     * @param customData Custom data to pass to the driver for asset creation
     */
    CreateContext(Repository& repo, Asset& asset, const CustomData& customData);

    /**
     * @brief Returns the custom data passed to the driver for asset creation
     */
    const CustomData& getCustomData() const { return customData; }

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
        if (!cast) {
            BL_LOG_ERROR << "Invalid custom data cast to " << typeid(T).name();
            return nullptr;
        }
        return cast;
    }

private:
    const CustomData& customData;
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
    ReadContext(Repository& repo, Asset& asset);

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
    WriteContext(Repository& repo, Asset& asset);

    /**
     * @brief Writes the file to asset storage
     *
     * @param filename The filename to use
     * @param buffer The content of the file to write
     * @return True if the file could be written, false otherwise
     */
    bool writeFile(std::string_view filename, std::span<const char> buffer) const;

    // TODO - stream based methods?

private:
    //
};

} // namespace as
} // namespace bl

#endif

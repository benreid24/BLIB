#ifndef BLIB_ASSETS_REPOSITORY_HPP
#define BLIB_ASSETS_REPOSITORY_HPP

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/Context.hpp>
#include <BLIB/Assets/Dependency.hpp>
#include <BLIB/Assets/DependencyList.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/Mode.hpp>
#include <BLIB/Assets/SourceLink.hpp>
#include <BLIB/Assets/State.hpp>
#include <BLIB/Assets/StreamCache.hpp>
#include <BLIB/Assets/TypedRef.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace as
{
/**
 * @brief The central asset repository. Stores all asset metadata, manages runtime bundling and
 *        loading, and provides access to the underlying assets
 *
 * @ingroup Assets
 */
class Repository {
public:
    /**
     * @brief Creates an empty asset repository
     *
     * @param mode The mode to create the repository in
     * @param path The directory to store assets in
     */
    Repository(Mode mode, const std::string& path);

    /**
     * @brief Destroys the repository, releasing all assets
     */
    ~Repository();

    /**
     * @brief Creates and returns a new asset with the given type and creation data
     *
     * @param type The tag of the type of asset to create
     * @param name The name of the asset
     * @param createData Custom data to pass to the driver for the asset type
     * @return A ref to the newly created asset. May be invalid if creation failed
     */
    Ref createAsset(std::string_view type, const std::string& name,
                    const CreateContext::CreateData& createData = {});

    /**
     * @brief Creates and returns a new asset with the given type and creation data
     *
     * @tparam T The payload type of the asset to create
     * @param name The name of the asset
     * @param createData Custom data to pass to the driver for the asset type
     * @return A TypedRef to the newly created asset. May be invalid if creation failed
     */
    template<typename T>
    TypedRef<T> createAsset(const std::string& name,
                            const CreateContext::CreateData& createData = {}) {
        std::string_view tag = getTagForType<T>();
        if (tag.empty()) { return TypedRef<T>(); }
        return TypedRef<T>(createAsset(tag, name, createData));
    }

    /**
     * @brief Returns a ref to the asset with the given UUID
     *
     * @param uuid The UUID of the asset to get
     * @param desiredState The desired state of the asset. Allows fetching metadata only if desired
     * @return A Ref to the asset. May be invalid if the asset does not exist or failed to load
     */
    Ref getAsset(util::UUID uuid, State desiredState = State::Loaded);

    /**
     * @brief Returns a ref to the asset with the given UUID
     *
     * @tparam T The type of payload the asset is expected to have
     * @param uuid The UUID of the asset to get
     * @return A TypedRef to the asset. May be invalid if the asset does not exist or failed to load
     */
    template<typename T>
    TypedRef<T> getTypedAsset(util::UUID uuid) {
        return TypedRef<T>(getAsset(uuid, State::Loaded));
    }

    /**
     * @brief Finds or creates an asset from the given source file path
     *
     * @param type The type tag of the asset to get
     * @param path The file path of the asset to get
     * @param desiredState The desired state of the asset
     * @return A Ref to the asset. May be invalid if the asset does not exist or failed to load
     */
    Ref getAssetFromSourcePath(std::string_view type, const std::string& path,
                               State desiredState = State::Loaded);

    /**
     * @brief Finds or creates an asset from the given source file path
     *
     * @tparam T The type of payload the asset is expected to have
     * @param path The file path of the asset to get
     * @param desiredState The desired state of the asset
     * @return A Ref to the asset. May be invalid if the asset does not exist or failed to load
     */
    template<typename T>
    TypedRef<T> getAssetFromSourcePath(const std::string& path,
                                       State desiredState = State::Loaded) {
        std::string_view tag = getTagForType<T>();
        if (tag.empty()) { return TypedRef<T>(); }
        return TypedRef<T>(getAssetFromSourcePath(tag, path, desiredState));
    }

    /**
     * @brief Performs a lookup of known static assets by file path
     *
     * @param path The path to search for
     * @return The UUID of the static asset with the given path
     */
    std::optional<util::UUID> findStaticAssetId(const std::string& path) const;

    /**
     * @brief Returns the dependency list for the asset with the given UUID
     *
     * @param uuid The UUID of the asset to get dependencies for
     * @return The dependency list for the asset
     */
    const std::vector<RepoDependency>& getDependencies(util::UUID uuid) const;

    /**
     * @brief Registers a new driver in the repository. Drivers provide support for different
     *        payload formats and allow arbitrary assets to be loaded by ID only
     *
     * @tparam TDriver The type of the driver to create
     * @tparam ...TArgs Signature of the driver constructor
     * @param type The type tag to identify assets associated with the driver
     * @param ...args Arguments to forward to the driver constructor
     */
    template<typename TDriver, typename... TArgs>
    void registerDriver(std::string_view type, TArgs&&... args) {
        static_assert(std::is_base_of_v<detail::DriverBase, TDriver>,
                      "TDriver must be a subclass of DriverBase");

        std::type_index typeIndex(typeid(typename TDriver::TPayload));
        std::unique_lock lock(driverMutex);
        if (driversByType.find(typeIndex) != driversByType.end()) {
            BL_LOG_ERROR << "Attempted to register multiple drivers for asset type "
                         << typeid(typename TDriver::TPayload).name();
            throw std::runtime_error("Driver for asset type is already registered");
        }
        drivers.emplace_back(std::make_unique<TDriver>(std::forward<TArgs>(args)...));
        driversByType[typeIndex]      = drivers.back().get();
        driversByName[type]           = drivers.back().get();
        drivers.back()->supportedType = type;
    }

    /**
     * @brief Returns the driver for the given type. May be nullptr
     *
     * @param type The tag name of the type to get the driver for
     * @return A pointer to the driver for the given type name, nullptr if not found
     */
    detail::DriverBase* getDriver(std::string_view type);

    /**
     * @brief Loads the repository from disk
     */
    bool loadRepository();

    /**
     * @brief Exports the repository to the given path, bundling assets and writing their data in
     *        game mode format
     *
     * @param path The directory to write to. Will be written to {path}/Assets
     * @return True if the export succeeded, false on error
     */
    bool exportRepository(const std::string& path);

    /**
     * @brief Release payloads for unused assets
     */
    void releaseUnused();

    /**
     * @brief Returns the mode the repository is operating in
     */
    Mode getMode() const { return mode; }

    /**
     * @brief Returns the directory that assets are saved and loaded from
     */
    const std::string& getAssetDirectory() const { return assetDirectory; }

    /**
     * @brief Reloads all assets from their source files if changed
     */
    void reloadFromSource();

private:
    mutable std::recursive_mutex assetMutex;
    const Mode mode;
    const std::string assetDirectory;
    std::unordered_map<util::UUID, Asset> assets;
    StreamCache streamCache;

    mutable std::mutex sourceLinkMutex;
    std::unordered_map<std::string, SourceLink> sourceLinks;

    std::shared_mutex driverMutex;
    std::vector<std::unique_ptr<detail::DriverBase>> drivers;
    std::unordered_map<std::type_index, detail::DriverBase*> driversByType;
    std::unordered_map<std::string_view, detail::DriverBase*> driversByName;

    std::mutex unloadQueueMutex;
    std::vector<util::UUID> unloadQueue;

    template<typename T>
    std::string_view getTagForType() {
        std::shared_lock lock(driverMutex);
        const auto it = driversByType.find(std::type_index(typeid(T)));
        if (it == driversByType.end()) {
            BL_LOG_ERROR << "Attempted to create asset with payload type " << typeid(T).name()
                         << " but no driver is registered for that type";
            return "";
        }
        return it->second->getSupportedType();
    }

    // used by Dependency
    void registerDependency(util::UUID uuid, std::string_view tag, util::UUID dependency);

    // used by Ref
    void queueUnload(util::UUID uuid);

    // used by Asset
    Asset* getDependencyForInit(util::UUID uuid);
    bool writeManifest();

    // used by Context
    PersistentStream* getPersistentStream(util::UUID uuid, const std::string& path);

    Ref createAssetShared(std::string_view type, const std::string& name,
                          const CreateContext::CreateData& createData, bool syncImmediate);
    bool writeManifestLocked();

    friend class detail::DependencyChain;
    friend class Ref;
    friend struct serial::SerializableObject<Repository>;
    friend class Asset;
    friend class detail::Context;
};

} // namespace as
} // namespace bl

#endif

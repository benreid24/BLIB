#ifndef BLIB_ASSETS_REPOSITORY_HPP
#define BLIB_ASSETS_REPOSITORY_HPP

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/Mode.hpp>
#include <BLIB/Assets/RepoAsset.hpp>
#include <BLIB/Assets/State.hpp>
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
     */
    Repository();

    /**
     * @brief Returns a ref to the asset with the given UUID
     *
     * @param uuid The UUID of the asset to get
     * @param desiredState The desired state of the asset. Allows fetching metadata only if desired
     * @return A Ref to the asset. May be invalid if the asset does not exist or failed to load
     */
    Ref getAsset(util::UUID uuid, State desiredState = State::Loaded);

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

        std::type_index typeIndex(typeid(typename TDriver::PayloadType));
        std::unique_lock lock(driverMutex);
        if (driversByType.find(typeIndex) != driversByType.end()) {
            BL_LOG_ERROR << "Attempted to register multiple drivers for asset type "
                         << typeid(typename TDriver::PayloadType).name();
            throw std::runtime_error("Driver for asset type is already registered");
        }
        drivers.emplace_back(std::make_unique<TDriver>(std::forward<TArgs>(args)...));
        driversByType[typeIndex] = drivers.back().get();
        driversByName[type]      = drivers.back().get();
    }

private:
    mutable std::shared_mutex assetMutex;
    Mode mode;
    std::unordered_map<util::UUID, RepoAsset> assets;

    std::mutex driverMutex;
    std::vector<std::unique_ptr<detail::DriverBase>> drivers;
    std::unordered_map<std::type_index, detail::DriverBase*> driversByType;
    std::unordered_map<std::string_view, detail::DriverBase*> driversByName;

    std::mutex unloadQueueMutex;
    std::vector<util::UUID> unloadQueue;

    // used by Dependency
    void registerDependency(util::UUID uuid, std::string_view tag, util::UUID dependency);

    // used by Ref
    void queueUnload(util::UUID uuid);

    friend class detail::DependencyChain;
    friend class Ref;
};

} // namespace as
} // namespace bl

#endif

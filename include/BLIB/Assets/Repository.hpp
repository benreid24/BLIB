#ifndef BLIB_ASSETS_REPOSITORY_HPP
#define BLIB_ASSETS_REPOSITORY_HPP

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/Mode.hpp>
#include <BLIB/Assets/State.hpp>
#include <BLIB/Assets/StoredDependency.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace as
{
class Repository {
public:
    Repository();

    Ref getAsset(util::UUID uuid, State desiredState = State::Loaded);

    const std::vector<StoredDependency>& getDependencies(util::UUID uuid) const;

    template<typename TDriver, typename... TArgs>
    void registerDriver(std::string_view type, TArgs&&... args) {
        static_assert(std::is_base_of_v<detail::DriverBase, TDriver>,
                      "TDriver must be a subclass of DriverBase");

        std::type_index typeIndex(typeid(typename TDriver::PayloadType));
        std::unique_lock lock(mutex);
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
    struct StoredAsset {
        std::optional<Asset> asset;
        std::vector<StoredDependency> dependencies;
    };

    std::mutex mutex;
    Mode mode;
    std::vector<std::unique_ptr<detail::DriverBase>> drivers;
    std::unordered_map<std::type_index, detail::DriverBase*> driversByType;
    std::unordered_map<std::string_view, detail::DriverBase*> driversByName;
    std::unordered_map<util::UUID, StoredAsset> assets;

    void registerDependency(util::UUID uuid, std::string_view tag, util::UUID dependency);

    friend class detail::DependencyChain;
};

} // namespace as
} // namespace bl

#endif

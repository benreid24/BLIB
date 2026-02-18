#ifndef BLIB_ASSETS_REPOSITORY_HPP
#define BLIB_ASSETS_REPOSITORY_HPP

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/Mode.hpp>
#include <BLIB/Assets/State.hpp>
#include <BLIB/Assets/StoredDependency.hpp>
#include <memory>
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

private:
    struct StoredAsset {
        std::optional<Asset> asset;
        std::vector<StoredDependency> dependencies;
    };

    Mode mode;
    std::unordered_map<std::type_index, std::unique_ptr<detail::DriverBase>> drivers;
    std::unordered_map<util::UUID, StoredAsset> assets;

    void registerDependency(util::UUID uuid, std::string_view tag, util::UUID dependency);

    friend class detail::DependencyChain;
};

} // namespace as
} // namespace bl

#endif

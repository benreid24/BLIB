#ifndef BLIB_ASSETS_REPOSITORY_HPP
#define BLIB_ASSETS_REPOSITORY_HPP

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/Mode.hpp>
#include <BLIB/Assets/State.hpp>
#include <memory>
#include <optional>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace as
{
class Repository {
public:
    Repository();

private:
    struct StoredAsset {
        std::optional<Asset> asset;
    };

    Mode mode;
    std::unordered_map<std::type_index, std::unique_ptr<Driver>> drivers;
    std::unordered_map<util::UUID, StoredAsset> assets;
};

} // namespace as
} // namespace bl

#endif

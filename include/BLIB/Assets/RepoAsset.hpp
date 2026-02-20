#ifndef BLIB_ASSETS_REPOASSET_HPP
#define BLIB_ASSETS_REPOASSET_HPP

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/RepoDependency.hpp>
#include <atomic>
#include <vector>

namespace bl
{
namespace as
{
class Repository;

/**
 * @brief Used by Repository to store assets in memory
 *
 * @ingroup Assets
 */
struct RepoAsset {
    Asset asset;
    std::vector<RepoDependency> dependencies;
    std::atomic<unsigned int> refCount;

    /**
     * @brief Creates an empty repository asset
     *
     * @param repo The repository this asset belongs to
     */
    RepoAsset(Repository& repo);
};

} // namespace as
} // namespace bl

#endif

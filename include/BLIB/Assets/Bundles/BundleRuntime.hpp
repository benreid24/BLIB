#ifndef BLIB_ASSETS_BUNDLES_BUNDLERUNTIME_HPP
#define BLIB_ASSETS_BUNDLES_BUNDLERUNTIME_HPP

#include <BLIB/Assets/Bundles/Manifest.hpp>
#include <BLIB/Assets/Bundles/MountedBundle.hpp>
#include <mutex>

namespace bl
{
namespace as
{
class Repository;

namespace bdl
{
class BundleRuntime {
public:
    BundleRuntime(Repository& repo, const std::string& bundlePath);

private:
    std::recursive_mutex mutex;
    Manifest manifest;
    std::unordered_map<util::UUID, MountedBundle> mountedBundles;
};

} // namespace bdl
} // namespace as
} // namespace bl

#endif

#include <BLIB/Assets/Bundles/BundleRuntime.hpp>

#include <BLIB/Assets/Bundles/RuntimePaths.hpp>
#include <BLIB/Util/FileUtil.hpp>

namespace bl
{
namespace as
{
namespace bdl
{
BundleRuntime::BundleRuntime(Repository& repo, const std::string& path)
: repo(repo)
, path(path) {}

bool BundleRuntime::initStream(stream::InputStream& stream, util::UUID uuid,
                               std::string_view path) {
    MountedBundle* bundle = getBundle(uuid);
    if (!bundle) { return false; }
    return bundle->initStream(stream, uuid, path);
}

bool BundleRuntime::initStreamDirect(stream::InputStream& stream, util::UUID uuid,
                                     std::string_view path) {
    MountedBundle* bundle = getBundle(uuid);
    if (!bundle) { return false; }
    return bundle->initStreamDirect(stream, uuid, path);
}

MountedBundle* BundleRuntime::getBundle(util::UUID uuid) {
    const auto assetIt = manifest.assetToBundle.find(uuid);
    if (assetIt == manifest.assetToBundle.end()) {
        BL_LOG_ERROR << "Asset " << uuid << " not found in manifest";
        return nullptr;
    }

    const auto bundleIt = mountedBundles.find(assetIt->second);
    if (bundleIt != mountedBundles.end()) { return &bundleIt->second; }

    const std::string bundlePath = RuntimePaths::getBundlePath(path, assetIt->second);
    return &mountedBundles.try_emplace(assetIt->second, repo, bundlePath).first->second;
}

void BundleRuntime::releaseStale() {
    std::erase_if(mountedBundles, [](const std::pair<util::UUID, MountedBundle>& bundle) -> bool {
        return bundle.second.isExpired();
    });
}

} // namespace bdl
} // namespace as
} // namespace bl

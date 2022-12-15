#include <BLIB/Resources/Bundling/BundleRuntime.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
BundleRuntime::BundleRuntime(const std::string& path)
: manifest(path) {}

bool BundleRuntime::initialize() {
    std::unique_lock lock(cleanMutex);

    return manifest.load();
}

bool BundleRuntime::getResource(const std::string& path, char** buf, std::size_t& len) {
    std::unique_lock lock(cleanMutex);

    Bundle* bundle = getOrLoadBundle(path);
    if (bundle) { return bundle->getResource(path, buf, len); }
    else {
        BL_LOG_ERROR << "Resource '" << path << "' not bundled";
        return false;
    }
}

bool BundleRuntime::resourceExists(const std::string& path) const {
    return manifest.containsFile(path);
}

void BundleRuntime::clean() {
    std::unique_lock lock(cleanMutex);

    for (auto it = bundles.begin(); it != bundles.end();) {
        auto jit = it++;
        if (jit->second.expired()) {
            BL_LOG_INFO << "Purging expired bundle: " << jit->first;
            bundles.erase(jit);
        }
    }
}

Bundle* BundleRuntime::getOrLoadBundle(const std::string& path) {
    const std::string& bundlePath = manifest.getBundleForFile(path);
    if (bundlePath.empty()) return nullptr;
    auto it = bundles.find(bundlePath);
    if (it == bundles.end()) { it = bundles.emplace(bundlePath, bundlePath).first; }
    return &it->second;
}

} // namespace bundle
} // namespace resource
} // namespace bl
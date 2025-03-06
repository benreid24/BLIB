#include <BLIB/Resources/FileSystem.hpp>
#include <BLIB/Resources/GarbageCollector.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <fstream>

namespace bl
{
namespace resource
{
namespace
{
std::optional<bundle::BundleRuntime> bundles;
std::unordered_map<std::string, std::vector<char>> persistentBuffers;
} // namespace

bool FileSystem::useBundle(const std::string& bundlePath) {
    bundles.emplace(bundlePath);
    if (!bundles.value().initialize()) return false;
    GarbageCollector::get().registerBundleRuntime(&bundles.value());
    return true;
}

bool FileSystem::isUsingBundle() { return bundles.has_value(); }

bool FileSystem::getData(const std::string& path, char** buffer, std::size_t& len) {
    if (bundles.has_value()) { return bundles.value().getResource(path, buffer, len); }
    else {
        if (!util::FileUtil::exists(path)) return false;

        // keep buffer in memory
        auto it = persistentBuffers.find(path);
        if (it == persistentBuffers.end()) { it = persistentBuffers.try_emplace(path).first; }
        if (!util::FileUtil::readFile(path, it->second)) return false;
        *buffer = it->second.data();
        len     = it->second.size();
        return true;
    }
}

void FileSystem::purgePersistentData(const std::string& path) { persistentBuffers.erase(path); }

bool FileSystem::resourceExists(const std::string& path) {
    return bundles.has_value() ? bundles.value().resourceExists(path) :
                                 util::FileUtil::exists(path);
}

} // namespace resource
} // namespace bl

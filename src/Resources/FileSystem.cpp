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
}

bool FileSystem::useBundle(const std::string& bundlePath) {
    bundles.emplace(bundlePath);
    if (!bundles.value().initialize()) return false;
    GarbageCollector::get().registerBundleRuntime(&bundles.value());
    return true;
}

bool FileSystem::getData(const std::string& path, std::vector<char>& data) {
    if (bundles.has_value()) { return bundles.value().getResource(path, data); }
    else {
        std::ifstream input(path.c_str(), std::ios::in | std::ios::binary);
        if (!input.good()) return false;
        input.seekg(0, std::ios_base::end);
        const std::streampos fileSize = input.tellg();
        data.resize(fileSize);
        input.seekg(0, std::ios_base::beg);
        input.read(data.data(), fileSize);
        return true;
    }
}

bool FileSystem::resourceExists(const std::string& path) {
    return bundles.has_value() ? bundles.value().resourceExists(path) :
                                 util::FileUtil::exists(path);
}

} // namespace resource
} // namespace bl

#include <BLIB/Resources/Bundling/Config.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Resources/Bundling/FileHandlers/DefaultHandler.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
namespace
{
DefaultHandler defaultHandler;
}

Config::Config(const std::string& outpath)
: outDir(outpath) {}

Config&& Config::withCatchAllDirectory(const std::string& call) {
    allFilesDir = call;
    return std::move(*this);
}

Config&& Config::addBundleSource(BundleSource&& src) {
    sources.emplace_back(std::forward<BundleSource>(src));
    return std::move(*this);
}

Config&& Config::addExcludePattern(const std::string& pattern) {
    excludePatterns.emplace_back(pattern, pattern.c_str());
    return std::move(*this);
}

const std::string& Config::outputDirectory() const { return outDir; }

const std::string& Config::catchAllDirectory() const { return allFilesDir; }

const std::vector<BundleSource>& Config::bundleSources() const { return sources; }

bool Config::includeFile(const std::string& path) const {
    if (excludeFiles.find(path) != excludeFiles.end()) {
        BL_LOG_DEBUG << "Excluding " << path << " based on handler-made exclude list";
        return false;
    }

    for (const auto& pattern : excludePatterns) {
        if (std::regex_match(path.c_str(), pattern.second)) {
            BL_LOG_DEBUG << "Excluding '" << path << "' based on exclude pattern '" << pattern.first
                         << "'";
            return false;
        }
    }
    return true;
}

FileHandler& Config::getFileHandler(const std::string& path) const {
    for (const auto& hp : handlers) {
        if (std::regex_match(path.c_str(), hp.first)) { return *hp.second; }
    }
    return defaultHandler;
}

} // namespace bundle
} // namespace resource
} // namespace bl

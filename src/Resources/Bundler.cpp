#include <BLIB/Resources/Bundler.hpp>

#include "Bundling/Utils.hpp"
#include <BLIB/Logging.hpp>
#include <BLIB/Resources/Bundling/BundleMetadata.hpp>
#include <BLIB/Resources/Bundling/Manifest.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <sstream>

namespace bl
{
namespace resource
{
namespace bundle
{
struct Stats {
    unsigned int bundleCount;
    unsigned int fileCount;
    unsigned int inputSize;
    unsigned int outputSize;

    Stats()
    : bundleCount(0)
    , fileCount(0)
    , inputSize(0)
    , outputSize(0) {}
};

class BundleCreator {
public:
    BundleCreator(const std::string& path, Config& config);
    ~BundleCreator();

    bool create(Stats& stats, Manifest& manifest);

private:
    const std::string bundlePath;
    const std::string tempPath;
    Config& config;
    std::stack<std::string, std::vector<std::string>> toBundle;
};
} // namespace bundle

namespace
{
std::string nextBundleName(const std::string& outDir, const std::string& sourcePath) {
    static unsigned int cb = 0;
    std::stringstream ss;
    ss << std::hex << (cb++) << std::hash<std::string>()(sourcePath) << ".bpack";
    return util::FileUtil::joinPath(outDir, ss.str());
}
} // namespace

Bundler::Bundler(bundle::Config&& config)
: config(std::forward<bundle::Config>(config)) {}

bool Bundler::createBundles() {
    BL_LOG_INFO << "Beginning bundle creation. Saving bundles to " << config.outputDirectory();

    if (!util::FileUtil::directoryExists(config.outputDirectory())) {
        util::FileUtil::createDirectory(config.outputDirectory());
        BL_LOG_INFO << "Created bundle directory";
    }
    else {
        BL_LOG_INFO << "Clearing out old bundle files";
        util::FileUtil::deleteDirectory(config.outputDirectory());
        util::FileUtil::createDirectory(config.outputDirectory());
        BL_LOG_INFO << "Deleted existing bundles";
    }

    bundle::Manifest manifest(config.outputDirectory());
    bundle::Stats stats;
    const auto doCreate = [this, &stats, &manifest](const std::string& path,
                                                    const std::string& og) -> bool {
        bundle::BundleCreator creator(path, config);
        if (!creator.create(stats, manifest)) {
            BL_LOG_ERROR << "Failed to create bundle for " << og << ", terminating";
            return false;
        }
        return true;
    };

    for (const bundle::BundleSource& src : config.bundleSources()) {
        if (src.policy == bundle::BundleSource::BundleAllFiles) {
            if (!doCreate(src.sourcePath, src.sourcePath)) { return false; }
        }
        else if (src.policy == bundle::BundleSource::CreateBundleForEachContainedRecursive) {
            const std::vector<std::string> all = util::FileUtil::listDirectory(src.sourcePath);
            for (const std::string& path : all) {
                if (config.includeFile(path)) {
                    if (!doCreate(path, src.sourcePath)) { return false; }
                }
            }
        }
        else if (src.policy == bundle::BundleSource::CreateBundleForEachContained) {
            const std::vector<std::string> folders =
                util::FileUtil::listDirectoryFolders(src.sourcePath);
            for (const std::string& dir : folders) {
                if (!doCreate(bl::util::FileUtil::joinPath(src.sourcePath, dir), src.sourcePath)) {
                    return false;
                }
            }
            const std::vector<std::string> files =
                util::FileUtil::listDirectory(src.sourcePath, "", false);
            for (const std::string& file : files) {
                if (!doCreate(file, src.sourcePath)) { return false; }
            }
        }
        else {
            BL_LOG_ERROR << "Bad bundle source policy: " << src.policy << ", skipping '"
                         << src.sourcePath << "'";
        }
    }

    if (util::FileUtil::directoryExists(config.catchAllDirectory())) {
        BL_LOG_INFO << "Bundling remaining files in top level directory: "
                    << config.catchAllDirectory();
        bundle::BundleCreator finalCreator(config.catchAllDirectory(), config);
        if (!finalCreator.create(stats, manifest)) {
            BL_LOG_ERROR << "Failed to bundle remaining files";
            return false;
        }
    }
    else if (!config.catchAllDirectory().empty()) {
        BL_LOG_WARN << "Catch-all directory " << config.catchAllDirectory() << " does not exist";
    }

    BL_LOG_INFO << "Bundled " << stats.fileCount << " files into " << stats.bundleCount
                << " bundles with a total size of " << formatSize(stats.outputSize)
                << ". Input files total size: " << formatSize(stats.inputSize);

    if (!manifest.save()) {
        BL_LOG_ERROR << "Failed to save manifest";
        return false;
    }
    BL_LOG_INFO << "Completed bundling";
    return true;
}

namespace bundle
{
BundleCreator::BundleCreator(const std::string& path, Config& cfg)
: bundlePath(nextBundleName(cfg.outputDirectory(), path))
, tempPath(util::FileUtil::joinPath(cfg.outputDirectory(), "temp.bundle"))
, config(cfg) {
    BL_LOG_INFO << "Starting bundle creation from path: " << path;
    if (util::FileUtil::directoryExists(path)) {
        std::vector<std::string> all = util::FileUtil::listDirectory(path);
        for (std::string& file : all) {
            if (config.includeFile(file)) { toBundle.emplace(std::move(file)); }
        }
        BL_LOG_INFO << "Found " << toBundle.size() << " files to bundle";
    }
    else {
        toBundle.emplace(path);
        BL_LOG_INFO << "Creating bundle from single file";
    }
}

BundleCreator::~BundleCreator() { util::FileUtil::deleteFile(tempPath); }

bool BundleCreator::create(Stats& stats, Manifest& manifest) {
    static std::unordered_set<std::string> failed;

    std::fstream tempfile(tempPath.c_str(), std::ios::binary | std::ios::out);

    BundleMetadata bundleManifest;
    unsigned int fileCount = 0;
    while (!toBundle.empty()) {
        const std::string path = toBundle.top();
        toBundle.pop();

        if (manifest.containsFile(path)) {
            BL_LOG_DEBUG << "Skipping already bundled file: " << path;
            continue;
        }

        FileHandler& handler = config.getFileHandler(path);
        FileHandlerContext ctx(config.excludeFiles, toBundle);
        const std::int64_t offset = tempfile.tellp();
        if (handler.processFile(path, tempfile, ctx)) {
            BL_LOG_DEBUG << "Added " << path << " to " << bundlePath;
            manifest.registerFileBundle(path, bundlePath);

            util::FileUtil::FileInfo info;
            ++fileCount;
            if (util::FileUtil::queryFileInfo(path, info)) { stats.inputSize += info.size; }

            const std::int64_t len = tempfile.tellp() - offset;
            bundleManifest.addFileInfo(path, {offset, len});
        }
        else {
            if (failed.find(path) == failed.end()) {
                failed.insert(path);
                BL_LOG_ERROR << "Failed to process file: " << path;
                tempfile.seekp(offset);
            }
        }
    }

    if (fileCount > 0) {
        BL_LOG_INFO << "Finalizing bundle " << bundlePath;

        tempfile.close();
        tempfile.open(tempPath.c_str(), std::ios::binary | std::ios::in);
        std::ofstream bundle(bundlePath.c_str(), std::ios::binary);

        // write bundle metadata
        if (!bundleManifest.save(bundle)) {
            BL_LOG_ERROR << "Failed to write bundle manifest";
            return false;
        }
        // copy bundled files from temp file
        std::istreambuf_iterator<char> begin_source(tempfile);
        std::istreambuf_iterator<char> end_source;
        std::ostreambuf_iterator<char> begin_dest(bundle);
        std::copy(begin_source, end_source, begin_dest);

        if (!bundle.good()) {
            BL_LOG_ERROR << "Failed to save final bundle";
            return false;
        }

        ++stats.bundleCount;
        stats.fileCount += fileCount;
        stats.outputSize += bundle.tellp();
        BL_LOG_INFO << "Completed bundle " << bundlePath << ". Bundled " << fileCount
                    << " files. Bundle size: " << formatSize(bundle.tellp());
    }
    else { BL_LOG_WARN << "Skipping empty bundle '" << bundlePath << "'"; }

    return true;
}

} // namespace bundle

} // namespace resource
} // namespace bl

#include <BLIB/Assets/StreamCache.hpp>

#include <BLIB/Assets/Asset.hpp>
#include <BLIB/Assets/Bundles/BundleRuntime.hpp>
#include <BLIB/Assets/EditorPaths.hpp>
#include <BLIB/Util/FileUtil.hpp>

namespace bl
{
namespace as
{
StreamCache::StreamCache(const std::string& repoRoot)
: repoRoot(repoRoot)
, runtime(nullptr) {}

void StreamCache::useBundles(bdl::BundleRuntime* rt) {
    if (!streams.empty()) {
        BL_LOG_ERROR << "Mounting bundle runtime prior to accessing asset data";
    }
    runtime = rt;
}

PersistentStream* StreamCache::getStream(Asset& asset, std::string_view localPath) {
    auto it = streams.find(asset.getUUID());
    if (it == streams.end()) {
        it = streams.emplace(asset.getUUID(), std::list<PersistentStream>()).first;
    }

    auto& stream = it->second.emplace_back();

    if (runtime) {
        if (!runtime->initStreamDirect(stream.getStream(), asset.getUUID(), localPath)) {
            it->second.pop_back();
            return nullptr;
        }
    }
    else {
        if (!stream.getStream().open(util::FileUtil::joinPath(
                EditorPaths::getAssetFilesPath(repoRoot, asset), std::string(localPath)))) {
            it->second.pop_back();
            return nullptr;
        }
    }

    return &stream;
}

void StreamCache::releaseStreams(util::UUID uuid) { streams.erase(uuid); }

} // namespace as
} // namespace bl

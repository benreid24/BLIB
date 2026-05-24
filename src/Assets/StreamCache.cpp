#include <BLIB/Assets/StreamCache.hpp>

#include <BLIB/Assets/Bundles/BundleRuntime.hpp>

namespace bl
{
namespace as
{
void StreamCache::useBundles(bdl::BundleRuntime* rt) {
    if (!streams.empty()) {
        BL_LOG_ERROR << "Mounting bundle runtime prior to accessing asset data";
    }
    runtime = rt;
}

PersistentStream* StreamCache::getStream(util::UUID uuid, const std::string& path) {
    auto it = streams.find(uuid);
    if (it == streams.end()) { it = streams.emplace(uuid, std::list<PersistentStream>()).first; }

    auto& stream = it->second.emplace_back();

    if (runtime) {
        if (!runtime->initStreamDirect(stream.getStream(), uuid, path)) {
            it->second.pop_back();
            return nullptr;
        }
    }
    else {
        if (!stream.getStream().open(path)) {
            it->second.pop_back();
            return nullptr;
        }
    }

    return &stream;
}

void StreamCache::releaseStreams(util::UUID uuid) { streams.erase(uuid); }

} // namespace as
} // namespace bl

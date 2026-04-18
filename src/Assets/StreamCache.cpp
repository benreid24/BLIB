#include <BLIB/Assets/StreamCache.hpp>

namespace bl
{
namespace as
{
PersistentStream* StreamCache::getStream(util::UUID uuid, const std::string& path) {
    // TODO - hook into bundles if in bundle mode

    auto it = streams.find(uuid);
    if (it == streams.end()) { it = streams.emplace(uuid, std::list<PersistentStream>()).first; }

    auto& stream = it->second.emplace_back();
    if (!stream.getStream().open(path)) {
        it->second.pop_back();
        return nullptr;
    }

    return &stream;
}

void StreamCache::releaseStreams(util::UUID uuid) {
    // TODO - hook into bundles if in bundle mode
    streams.erase(uuid);
}

} // namespace as
} // namespace bl

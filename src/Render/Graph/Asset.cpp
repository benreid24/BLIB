#include <BLIB/Render/Graph/Asset.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
Asset::Asset(std::string_view tag)
: tag(tag)
, created(false)
, refCount(0) {}

void Asset::create(engine::Engine& engine, Renderer& renderer) {
    if (!created) {
        created = true;
        doCreate(engine, renderer);
    }
}

bool Asset::isOwnedBy(GraphAssetPool* pool) {
    return std::find(owners.begin(), owners.end(), pool) != owners.end();
}

void Asset::addOwner(GraphAssetPool* pool) { owners.emplace_back(pool); }

void Asset::removeOwner(GraphAssetPool* pool) {
    const auto it = std::find(owners.begin(), owners.end(), pool);
    if (it != owners.end()) { owners.erase(it); }
}

} // namespace rg
} // namespace rc
} // namespace bl

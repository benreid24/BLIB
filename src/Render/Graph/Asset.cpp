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
, refCount(0)
, mode(InputMode::Unset)
, external(false) {}

void Asset::create(engine::Engine& engine, Renderer& renderer, RenderTarget* observer) {
    if (!created) {
        created = true;
        doCreate(engine, renderer, observer);
    }
}

void Asset::prepareForInput(const ExecutionContext& ctx) {
    if (mode != InputMode::Input) {
        mode = InputMode::Input;
        doPrepareForInput(ctx);
    }
}

void Asset::prepareForOutput(const ExecutionContext& ctx) {
    if (mode != InputMode::Output) {
        mode = InputMode::Output;
        doPrepareForOutput(ctx);
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

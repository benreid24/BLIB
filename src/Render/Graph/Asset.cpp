#include <BLIB/Render/Graph/Asset.hpp>

#include <algorithm>

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

bool Asset::create(engine::Engine& engine, Renderer& renderer, RenderTarget* observer) {
    if (!created) {
        created = true;
        doCreate(engine, renderer, observer);
        return true;
    }
    return false;
}

void Asset::prepareForInput(const ExecutionContext& ctx) {
    if (mode != InputMode::Input) {
        mode = InputMode::Input;
        doPrepareForInput(ctx);
    }
}

void Asset::startOutput(const ExecutionContext& ctx) {
    if (mode != InputMode::OutputStart) {
        mode = InputMode::OutputStart;
        doStartOutput(ctx);
    }
}

void Asset::endOutput(const ExecutionContext& ctx) {
    if (mode != InputMode::OutputEnd) {
        mode = InputMode::OutputEnd;
        doEndOutput(ctx);
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

void Asset::reset() { mode = InputMode::Unset; }

} // namespace rg
} // namespace rc
} // namespace bl

#include <BLIB/Render/Graph/Assets/SceneAsset.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
SceneAsset::SceneAsset(Scene* s)
: Asset(rg::AssetTags::SceneObjectsInput)
, scene(s) {}

void SceneAsset::doCreate(engine::Engine&, Renderer&, RenderTarget*) {
    // noop
}

void SceneAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop
}

void SceneAsset::doPrepareForOutput(const rg::ExecutionContext&) {
    // noop
}

} // namespace rgi
} // namespace rc
} // namespace bl

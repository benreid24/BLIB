#include <BLIB/Render/Graph/Assets/SceneAsset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
SceneAsset::SceneAsset(Scene* s, std::string_view tag)
: Asset(tag, false)
, scene(s) {}

void SceneAsset::doCreate(engine::Engine&, Renderer&, RenderTarget*) {
    // noop
}

void SceneAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop
}

void SceneAsset::doStartOutput(const rg::ExecutionContext&) {
    // noop
}

void SceneAsset::doEndOutput(const rg::ExecutionContext&) {
    // noop
}

} // namespace rgi
} // namespace rc
} // namespace bl

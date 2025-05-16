#include <BLIB/Render/Graph/Tasks/ShadowMapTask.hpp>

#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ShadowMapTask::ShadowMapTask()
: scene(nullptr)
, shadowMaps(nullptr) {
    assetTags.requiredInputs.push_back(rg::AssetTags::SceneInput);
    assetTags.outputs.emplace_back(rg::AssetTags::ShadowMaps, rg::TaskOutput::CreatedByTask);
}

void ShadowMapTask::create(engine::Engine&, Renderer&, Scene* s) {
    scene = dynamic_cast<scene::Scene3D*>(s);
    if (!scene) { throw std::runtime_error("ShadowMapTask can only be used with Scene3D"); }
}

void ShadowMapTask::onGraphInit() {
    shadowMaps = dynamic_cast<ShadowMapAsset*>(&assets.outputs[0]->asset.get());
    if (!shadowMaps) { throw std::runtime_error("ShadowMapTask requires ShadowMapAsset input"); }
}

void ShadowMapTask::execute(const rg::ExecutionContext&, rg::Asset*) {
    // TODO - implement shadow map rendering
}

} // namespace rgi
} // namespace rc
} // namespace bl

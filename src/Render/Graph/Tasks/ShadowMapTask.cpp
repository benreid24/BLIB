#include <BLIB/Render/Graph/Tasks/ShadowMapTask.hpp>

#include <BLIB/Render/Config/RenderPassIds.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/ExtraContexts.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
ShadowMapTask::ShadowMapTask()
: renderer(nullptr)
, scene(nullptr)
, shadowMaps(nullptr) {
    assetTags.requiredInputs.push_back(rg::AssetTags::SceneInput);
    assetTags.outputs.emplace_back(rg::AssetTags::ShadowMaps, rg::TaskOutput::CreatedByTask);
}

void ShadowMapTask::create(engine::Engine&, Renderer& r, Scene* s) {
    renderer = &r;
    scene    = dynamic_cast<scene::Scene3D*>(s);
    if (!scene) { throw std::runtime_error("ShadowMapTask can only be used with Scene3D"); }
}

void ShadowMapTask::onGraphInit() {
    shadowMaps = dynamic_cast<ShadowMapAsset*>(&assets.outputs[0]->asset.get());
    if (!shadowMaps) { throw std::runtime_error("ShadowMapTask requires ShadowMapAsset input"); }
}

void ShadowMapTask::execute(const rg::ExecutionContext& execCtx, rg::Asset*) {
    const auto res = renderer->getSettings().getShadowMapResolution();
    const VkViewport viewport{.x        = 0,
                              .y        = 0,
                              .width    = static_cast<float>(res.width),
                              .height   = static_cast<float>(res.height),
                              .minDepth = 0.f,
                              .maxDepth = 1.f};
    const VkClearValue clearColor{.depthStencil = {.depth = 1.f, .stencil = 0}};

    lgt::Scene3DLighting& lighting = scene->getLighting();

    // sun shadow map
    scene::SceneRenderContext sunCtx(execCtx.commandBuffer,
                                     execCtx.observerIndex,
                                     viewport,
                                     RenderPhase::ShadowMap,
                                     cfg::RenderPassIds::ShadowMapPass,
                                     false);
    scene::ctx::ShadowMapContext sunShadowCtx{.lightType  = scene::ctx::ShadowMapContext::SunLight,
                                              .lightIndex = 0};
    sunCtx.setExtraContext(&sunShadowCtx);

    shadowMaps->getSpotShadowFramebuffer(0).beginRender(
        execCtx.commandBuffer,
        {.offset = {0, 0}, .extent = {res.width, res.height}},
        &clearColor,
        1,
        true,
        nullptr,
        true);
    scene->renderScene(sunCtx);
    shadowMaps->getSpotShadowFramebuffer(0).finishRender(execCtx.commandBuffer);

    // spot light shadows
    for (unsigned int i = 0; i < lighting.getSpotShadowCount(); ++i) {
        scene::SceneRenderContext spotCtx(execCtx.commandBuffer,
                                          execCtx.observerIndex,
                                          viewport,
                                          RenderPhase::ShadowMap,
                                          cfg::RenderPassIds::ShadowMapPass,
                                          false);
        scene::ctx::ShadowMapContext spotShadowCtx{
            .lightType = scene::ctx::ShadowMapContext::SpotLight, .lightIndex = i};
        spotCtx.setExtraContext(&spotShadowCtx);

        auto& fb = shadowMaps->getSpotShadowFramebuffer(i + 1);
        fb.beginRender(execCtx.commandBuffer,
                       {.offset = {0, 0}, .extent = {res.width, res.height}},
                       &clearColor,
                       1,
                       true,
                       nullptr,
                       true);
        scene->renderScene(spotCtx);
        fb.finishRender(execCtx.commandBuffer);
    }

    // point light shadows
    for (unsigned int i = 0; i < lighting.getPointShadowCount(); ++i) {
        scene::SceneRenderContext pointCtx(execCtx.commandBuffer,
                                           execCtx.observerIndex,
                                           viewport,
                                           RenderPhase::ShadowPointMap,
                                           cfg::RenderPassIds::ShadowMapPass,
                                           false);
        scene::ctx::ShadowMapContext pointShadowCtx{
            .lightType = scene::ctx::ShadowMapContext::PointLight, .lightIndex = i};
        pointCtx.setExtraContext(&pointShadowCtx);

        auto& fb = shadowMaps->getPointShadowFramebuffer(i);
        fb.beginRender(execCtx.commandBuffer,
                       {.offset = {0, 0}, .extent = {res.width, res.height}},
                       &clearColor,
                       1,
                       true,
                       nullptr,
                       true);
        scene->renderScene(pointCtx);
        fb.finishRender(execCtx.commandBuffer);
    }
}

} // namespace rgi
} // namespace rc
} // namespace bl

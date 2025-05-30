#include <BLIB/Render/Resources/MaterialPipelineCache.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace res
{
MaterialPipelineCache::MaterialPipelineCache(Renderer& renderer)
: renderer(renderer)
, nextId(DynamicMaterialPipelineIdStart) {}

mat::MaterialPipeline& MaterialPipelineCache::createPipeline(
    std::uint32_t pipelineId, mat::MaterialPipelineSettings&& settings) {
    auto result = pipelines.try_emplace(pipelineId, renderer, pipelineId, std::move(settings));
    return result.first->second;
}

mat::MaterialPipeline& MaterialPipelineCache::getOrCreatePipeline(
    mat::MaterialPipelineSettings&& settings) {
    for (auto& p : pipelines) {
        if (p.second.getSettings() == settings) { return p.second; }
    }

    const std::uint32_t id = nextId++;
    return createPipeline(id, std::move(settings));
}

mat::MaterialPipeline& MaterialPipelineCache::getPipeline(std::uint32_t pipelineId) {
    return pipelines.at(pipelineId);
}

bool MaterialPipelineCache::pipelineExists(std::uint32_t pipelineId) const {
    return pipelines.find(pipelineId) != pipelines.end();
}

void MaterialPipelineCache::createBuiltins() {
    const auto make = [this](std::uint32_t materialId, std::uint32_t pipelineId) {
        createPipeline(materialId, mat::MaterialPipelineSettings(pipelineId).build());
    };
    const auto makeOverlayPair = [this](std::uint32_t materialId,
                                        std::uint32_t pipelineId,
                                        std::uint32_t overlayPipelineId) {
        createPipeline(materialId,
                       mat::MaterialPipelineSettings(pipelineId)
                           .withRenderPhasePipelineOverride(RenderPhase::Overlay, overlayPipelineId)
                           .build());
    };
    const auto make3D = [this](std::uint32_t materialIds,
                               std::uint32_t pipelineId,
                               std::uint32_t overlayPipelineId,
                               std::uint32_t shadowMapPipelineId,
                               std::uint32_t pointShadowMapPipelineId) {
        createPipeline(
            materialIds,
            mat::MaterialPipelineSettings(pipelineId)
                .withRenderPhasePipelineOverride(RenderPhase::Overlay, overlayPipelineId)
                .withRenderPhasePipelineOverride(RenderPhase::ShadowMap, shadowMapPipelineId)
                .withRenderPhasePipelineOverride(RenderPhase::ShadowPointMap,
                                                 pointShadowMapPipelineId)
                .build());
    };

    using MId = Config::MaterialPipelineIds;
    using PId = Config::PipelineIds;
    using B   = mat::MaterialPipelineSettings::PhasePipelineOverride;

    make3D(MId::Mesh3D,
           PId::LitMesh3D,
           PId::UnlitMesh3D,
           PId::ShadowMapRegular,
           PId::PointShadowMapRegular);
    make3D(MId::Mesh3DMaterial,
           PId::LitMesh3DMaterial,
           PId::UnlitMesh3DMaterial,
           PId::ShadowMapRegular,
           PId::PointShadowMapRegular);
    make3D(MId::Mesh3DSkinned,
           PId::LitMesh3DSkinned,
           PId::UnlitMesh3DSkinned,
           PId::ShadowMapSkinned,
           PId::PointShadowMapSkinned);
    makeOverlayPair(MId::Geometry2D, PId::Lit2DGeometry, PId::Unlit2DGeometry);
    makeOverlayPair(MId::Geometry2DSkinned, PId::LitSkinned2DGeometry, PId::UnlitSkinned2DGeometry);
    makeOverlayPair(MId::Slideshow2D, PId::SlideshowLit, PId::SlideshowUnlit);
    make(MId::Text, PId::Text);
    make(MId::Lines2D, PId::Lines2D);
    createPipeline(MId::Skybox,
                   mat::MaterialPipelineSettings(PId::Skybox)
                       .withRenderPhasePipelineOverride(RenderPhase::Overlay, B::None)
                       .build());
}

} // namespace res
} // namespace rc
} // namespace bl

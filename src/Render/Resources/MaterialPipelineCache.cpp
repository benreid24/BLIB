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

    using MId = Config::MaterialPipelineIds;
    using PId = Config::PipelineIds;

    makeOverlayPair(MId::Mesh3D, PId::LitMesh3D, PId::UnlitMesh3D);
    makeOverlayPair(MId::Mesh3DSkinned, PId::LitSkinnedMesh3D, PId::UnlitSkinnedMesh3D);
    makeOverlayPair(MId::Geometry2D, PId::Lit2DGeometry, PId::Unlit2DGeometry);
    makeOverlayPair(MId::Geometry2DSkinned, PId::LitSkinned2DGeometry, PId::UnlitSkinned2DGeometry);
    makeOverlayPair(MId::Slideshow2D, PId::SlideshowLit, PId::SlideshowUnlit);
    make(MId::Text, PId::Text);
    make(MId::Lines2D, PId::Lines2D);
}

} // namespace res
} // namespace rc
} // namespace bl

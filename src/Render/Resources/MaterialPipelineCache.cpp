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
    for (std::uint32_t i = 0; i < 110; ++i) {
        if (renderer.pipelineCache().pipelineExists(i)) {
            createPipeline(i, mat::MaterialPipelineSettings(i).build());
        }
    }
}

} // namespace res
} // namespace rc
} // namespace bl

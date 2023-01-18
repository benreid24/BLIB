#include <BLIB/Render/Resources/PipelineCache.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
PipelineCache::PipelineCache(Renderer& r)
: renderer(r) {
    // TODO - leverage VkPipelineCache?
    createBuiltins();
}

void PipelineCache::cleanup() { cache.clear(); }

Pipeline& PipelineCache::createPipline(std::uint32_t id, PipelineParameters&& params) {
    const auto insertResult =
        cache.try_emplace(id, renderer, std::forward<PipelineParameters>(params));
    if (!insertResult.second) { BL_LOG_WARN << "Pipeline with id " << id << " already exists"; }
    return insertResult.first->second;
}

Pipeline& PipelineCache::getPipeline(std::uint32_t id) {
    auto it = cache.find(id);
    if (it == cache.end()) {
        BL_LOG_CRITICAL << "Failed to find pipeline with id: " << id;
        throw std::runtime_error("Failed to find pipeline");
    }
    return it->second;
}

void PipelineCache::createBuiltins() {
    // TODO - create actual built-in pipelines
    createPipline(Config::PipelineIds::OpaqueMeshes,
                  PipelineParameters(Config::RenderPassIds::Opaque)
                      .withShaders(Config::BuiltInShaderIds::TestVertexShader,
                                   Config::BuiltInShaderIds::TestFragmentShader)
                      .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                      .build());
}

} // namespace render
} // namespace bl

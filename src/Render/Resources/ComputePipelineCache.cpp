#include <BLIB/Render/Resources/ComputePipelineCache.hpp>

#include <stdexcept>

namespace bl
{
namespace rc
{
namespace res
{
ComputePipelineCache::ComputePipelineCache(Renderer& renderer)
: renderer(renderer) {}

void ComputePipelineCache::cleanup() { cache.clear(); }

vk::ComputePipeline& ComputePipelineCache::createPipeline(std::uint32_t pipelineId,
                                                          vk::ComputePipelineParameters&& params) {
    if (cache.find(pipelineId) != cache.end()) {
        throw std::runtime_error("Attempted to create compute pipeline with duplicate id");
    }
    const auto it =
        cache.emplace(pipelineId, vk::ComputePipeline(renderer, pipelineId, std::move(params)))
            .first;
    return it->second;
}

vk::ComputePipeline* ComputePipelineCache::getPipelineMaybe(std::uint32_t pipelineId) {
    const auto it = cache.find(pipelineId);
    if (it == cache.end()) { return nullptr; }
    return &it->second;
}

vk::ComputePipeline& ComputePipelineCache::getPipeline(std::uint32_t pipelineId) {
    return *getPipelineMaybe(pipelineId);
}

bool ComputePipelineCache::pipelineExists(std::uint32_t pipelineId) const {
    return cache.find(pipelineId) != cache.end();
}

void ComputePipelineCache::createBuiltins() {
    // TODO - create built-in compute pipelines here
}

} // namespace res
} // namespace rc
} // namespace bl

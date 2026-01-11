#include <BLIB/Render/Resources/ComputePipelineCache.hpp>

#include <BLIB/Render/Config/ComputePipelineIds.hpp>
#include <BLIB/Render/Config/ShaderIds.hpp>
#include <BLIB/Render/Descriptors/Builtin/AutoExposureWorkBuffer.hpp>
#include <BLIB/Render/Descriptors/Builtin/GlobalDataFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/InputAttachmentFactory.hpp>
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
    const auto it = cache.try_emplace(pipelineId, renderer, pipelineId, std::move(params)).first;
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
    using RenderedSceneSet = dsi::InputAttachmentFactory<1, 0, VK_SHADER_STAGE_COMPUTE_BIT>;

    createPipeline(cfg::ComputePipelineIds::AutoExposureAccumulate,
                   vk::ComputePipelineParameters()
                       .withShader(cfg::ShaderIds::AutoExposureAccumulate)
                       .addDescriptorSet<dsi::GlobalDataFactory>()
                       .addDescriptorSet<RenderedSceneSet>()
                       .addDescriptorSet<dsi::AutoExposureBufferFactory>()
                       .build());

    createPipeline(cfg::ComputePipelineIds::AutoExposureAdjust,
                   vk::ComputePipelineParameters()
                       .withShader(cfg::ShaderIds::AutoExposureAdjust)
                       .addDescriptorSet<dsi::GlobalDataFactory>()
                       .addDescriptorSet<RenderedSceneSet>()
                       .addDescriptorSet<dsi::AutoExposureBufferFactory>()
                       .build());
}

} // namespace res
} // namespace rc
} // namespace bl

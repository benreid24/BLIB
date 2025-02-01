#include <BLIB/Render/Materials/MaterialPipeline.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace mat
{
MaterialPipeline::MaterialPipeline(Renderer& renderer, std::uint32_t id,
                                   const MaterialPipelineSettings& s)
: renderer(renderer)
, id(id)
, settings(s) {
    mainPipeline = resolvePipeline(settings.mainPipeline);
    for (unsigned int i = 0; i < Config::MaxRenderPhases; ++i) {
        pipelines[i] = resolvePipeline(settings.renderPhaseOverrides[i]);
    }
}

void MaterialPipeline::bind(VkCommandBuffer commandBuffer, RenderPhase phase,
                            std::uint32_t renderPassId) {
    pipelines[renderPhaseIndex(phase)]->bind(commandBuffer, renderPassId);
}

vk::Pipeline* MaterialPipeline::resolvePipeline(MaterialPipelineSettings::PipelineInfo& info) {
    if (info.pipelineParams != nullptr) {
        vk::Pipeline* result =
            &renderer.pipelineCache().getOrCreatePipeline(info.pipelineParams->build());
        info.pipelineParams = nullptr;
        info.id             = result->getId();
        return result;
    }
    if (info.id != Config::PipelineIds::None) {
        return &renderer.pipelineCache().getPipeline(info.id);
    }
    if (info.overrideBehavior != MaterialPipelineSettings::None) {
        switch (info.overrideBehavior) {
        case MaterialPipelineSettings::FragmentNoop: {
            vk::PipelineParameters params = mainPipeline->getCreationParameters();
            params.removeShader(VK_SHADER_STAGE_FRAGMENT_BIT);
            return &renderer.pipelineCache().getOrCreatePipeline(params.build());
        }
        default: {
            BL_LOG_CRITICAL << "Unknown pipeline override behavior: " << info.overrideBehavior;
            throw std::runtime_error("Unknown pipeline override behavior");
        }
        }
    }

    return mainPipeline;
}

} // namespace mat
} // namespace rc
} // namespace bl

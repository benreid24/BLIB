#include <BLIB/Render/Materials/MaterialPipeline.hpp>

#include <BLIB/Render/Config/PipelineIds.hpp>
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
    for (unsigned int i = 0; i < cfg::Limits::MaxRenderPhases; ++i) {
        pipelines[i] = resolvePipeline(settings.renderPhaseOverrides[i]);
    }
}

VkPipeline MaterialPipeline::getRawPipeline(RenderPhase phase, std::uint32_t renderPassId,
                                            std::uint32_t objectSpec) const {
    const unsigned int i     = renderPhaseIndex(phase);
    const std::uint32_t spec = phase == RenderPhase::Default ?
                                   objectSpec :
                                   settings.renderPhaseOverrides[i].specialization;
    return pipelines[i]->rawPipeline(renderPassId, spec);
}

bool MaterialPipeline::bind(VkCommandBuffer commandBuffer, RenderPhase phase,
                            std::uint32_t renderPassId, std::uint32_t objectSpec) {
    const unsigned int i = renderPhaseIndex(phase);
    vk::Pipeline* p      = pipelines[i];
    if (!p) { return false; }

    const std::uint32_t spec = phase == RenderPhase::Default ?
                                   objectSpec :
                                   settings.renderPhaseOverrides[i].specialization;

    p->bind(commandBuffer, renderPassId, spec);
    return true;
}

vk::Pipeline* MaterialPipeline::resolvePipeline(MaterialPipelineSettings::PipelineInfo& info) {
    if (info.pipelineParams != nullptr) {
        vk::Pipeline* result =
            &renderer.pipelineCache().getOrCreatePipeline(info.pipelineParams->build());
        info.pipelineParams = nullptr;
        info.id             = result->getId();
        return result;
    }
    if (info.id != cfg::PipelineIds::None) {
        return &renderer.pipelineCache().getPipeline(info.id);
    }
    if (info.overrideBehavior != MaterialPipelineSettings::NoOverride) {
        switch (info.overrideBehavior) {
        case MaterialPipelineSettings::NotRendered:
            return nullptr;
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

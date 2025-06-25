#include <BLIB/Render/Materials/MaterialPipelineSettings.hpp>

#include <BLIB/Render/Config/PipelineIds.hpp>

namespace bl
{
namespace rc
{
namespace mat
{
MaterialPipelineSettings::MaterialPipelineSettings(std::uint32_t pipelineId)
: mainPipeline(pipelineId, 0)
, phases(RenderPhase::All) {
    renderPhaseOverrides.fill(NoOverride);
}

MaterialPipelineSettings::MaterialPipelineSettings(vk::PipelineParameters* pipelineParams)
: mainPipeline(pipelineParams, 0)
, phases(RenderPhase::All) {}

MaterialPipelineSettings::MaterialPipelineSettings(const MaterialPipelineSettings& copy)
: mainPipeline(copy.mainPipeline)
, phases(copy.phases)
, renderPhaseOverrides(copy.renderPhaseOverrides) {}

MaterialPipelineSettings& MaterialPipelineSettings::withPipeline(std::uint32_t pid) {
    mainPipeline = PipelineInfo(pid, 0);
    return *this;
}

MaterialPipelineSettings& MaterialPipelineSettings::withPipeline(vk::PipelineParameters* params) {
    mainPipeline = PipelineInfo(params, 0);
    return *this;
}

MaterialPipelineSettings& MaterialPipelineSettings::withRenderPhase(RenderPhase phase) {
    phases = phase;
    return *this;
}

MaterialPipelineSettings& MaterialPipelineSettings::withRenderPhasePipelineOverride(
    RenderPhase phase, PhasePipelineOverride overrideBehavior) {
    renderPhaseOverrides[renderPhaseIndex(phase)] = PipelineInfo(overrideBehavior);
    return *this;
}

MaterialPipelineSettings& MaterialPipelineSettings::withRenderPhasePipelineOverride(
    RenderPhase phase, std::uint32_t pipelineId, std::uint32_t specialization) {
    renderPhaseOverrides[renderPhaseIndex(phase)] = PipelineInfo(pipelineId, specialization);
    return *this;
}

MaterialPipelineSettings& MaterialPipelineSettings::withRenderPhasePipelineOverride(
    RenderPhase phase, vk::PipelineParameters* params, std::uint32_t specialization) {
    renderPhaseOverrides[renderPhaseIndex(phase)] = PipelineInfo(params, specialization);
    return *this;
}

MaterialPipelineSettings&& MaterialPipelineSettings::build() {
    const auto validatePipeline = [this](const PipelineInfo& info) {
        if (info.pipelineParams != nullptr) {
            if (info.id != cfg::PipelineIds::None) {
                BL_LOG_CRITICAL
                    << "Material pipeline cannot have both a pipeline id and parameters";
                return false;
            }
            if (info.overrideBehavior != PhasePipelineOverride::NoOverride) {
                BL_LOG_CRITICAL << "Material pipeline cannot have both an override and parameters";
                return false;
            }

            info.pipelineParams->build();
        }

        if (info.id != cfg::PipelineIds::None) {
            if (info.overrideBehavior != PhasePipelineOverride::NoOverride) {
                BL_LOG_CRITICAL << "Material pipeline cannot have both an override and id";
                return false;
            }
            if (info.pipelineParams != nullptr) {
                BL_LOG_CRITICAL
                    << "Material pipeline cannot have both a pipeline id and parameters";
                return false;
            }
        }

        if (info.overrideBehavior != PhasePipelineOverride::NoOverride) {
            if (info.pipelineParams != nullptr) {
                BL_LOG_CRITICAL << "Material pipeline cannot have both an override behavior and "
                                   "pipeline parameters";
                return false;
            }
            if (info.id != cfg::PipelineIds::None) {
                BL_LOG_CRITICAL
                    << "Material pipeline cannot have both an override behavior and pipeline id";
                return false;
            }
        }

        return true;
    };

    if (mainPipeline.pipelineParams == nullptr && mainPipeline.id == cfg::PipelineIds::None &&
        mainPipeline.overrideBehavior == PhasePipelineOverride::NoOverride) {
        BL_LOG_CRITICAL << "Material pipeline is missing a valid pipeline";
        throw std::runtime_error("Invalid material pipeline settings");
    }

    if (!validatePipeline(mainPipeline)) {
        throw std::runtime_error("Invalid material pipeline settings");
    }

    for (unsigned int i = 0; i < 8; ++i) {
        if (!validatePipeline(renderPhaseOverrides[i])) {
            throw std::runtime_error("Invalid material pipeline settings for phase: " +
                                     std::to_string(0x1 << i));
        }
    }

    return std::move(*this);
}

bool MaterialPipelineSettings::operator==(const MaterialPipelineSettings& right) const {
    if (mainPipeline != right.mainPipeline) { return false; }
    if (phases != right.phases) { return false; }
    for (unsigned int i = 0; i < cfg::Limits::MaxRenderPhases; ++i) {
        if (renderPhaseOverrides[i] != right.renderPhaseOverrides[i]) { return false; }
    }
    return true;
}

bool MaterialPipelineSettings::operator!=(const MaterialPipelineSettings& right) const {
    return !this->operator==(right);
}

} // namespace mat
} // namespace rc
} // namespace bl

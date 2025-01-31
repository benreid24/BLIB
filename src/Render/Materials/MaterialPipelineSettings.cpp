#include <BLIB/Render/Materials/MaterialPipelineSettings.hpp>

#include <BLIB/Render/Config.hpp>

namespace bl
{
namespace rc
{
namespace mat
{
MaterialPipelineSettings::ShaderInfo::ShaderInfo()
: path("")
, entrypoint("main") {}

MaterialPipelineSettings::MaterialPipelineSettings(std::uint32_t pipelineId)
: pipelineId(pipelineId)
, pipelineParams(nullptr)
, phases(RenderPhase::All) {}

MaterialPipelineSettings::MaterialPipelineSettings(vk::PipelineParameters* pipelineParams)
: pipelineId(Config::PipelineIds::None)
, pipelineParams(pipelineParams)
, phases(RenderPhase::All) {}

MaterialPipelineSettings::MaterialPipelineSettings(const MaterialPipelineSettings& copy)
: pipelineId(copy.pipelineId)
, pipelineParams(copy.pipelineParams)
, phases(copy.phases)
, shadowFragmentShaderOverride(copy.shadowFragmentShaderOverride) {}

MaterialPipelineSettings& MaterialPipelineSettings::withPipeline(std::uint32_t pid) {
    pipelineId     = pid;
    pipelineParams = nullptr;
    return *this;
}

MaterialPipelineSettings& MaterialPipelineSettings::withPipeline(vk::PipelineParameters* params) {
    pipelineId     = Config::PipelineIds::None;
    pipelineParams = params;
    return *this;
}

MaterialPipelineSettings& MaterialPipelineSettings::withRenderPhase(RenderPhase phase) {
    phases = phase;
    return *this;
}

MaterialPipelineSettings& MaterialPipelineSettings::withShadowFragmentShaderOverride(
    const std::string& path, const std::string& entrypoint) {
    shadowFragmentShaderOverride.path       = path;
    shadowFragmentShaderOverride.entrypoint = entrypoint;
    return *this;
}

MaterialPipelineSettings&& MaterialPipelineSettings::build() {
    if (pipelineId == Config::PipelineIds::None && !pipelineParams) {
        throw std::runtime_error("No pipeline set");
    }
    return std::move(*this);
}

} // namespace mat
} // namespace rc
} // namespace bl

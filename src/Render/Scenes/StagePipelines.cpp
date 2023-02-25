#include <BLIB/Render/Scenes/StagePipelines.hpp>

#include <BLIB/Render/Config.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{

StagePipelines::StagePipelines() {
    pipelines.fill(Config::PipelineIds::None);
    next.fill(Config::PipelineIds::None);
}

bool StagePipelines::hasDiff() const {
    for (unsigned int i = 0; i < pipelines.size(); ++i) {
        if (pipelines[i] != next[i]) return true;
    }
    return false;
}

StagePipelines::Diff StagePipelines::nextDiff() {
    unsigned int i = 0;
    for (; i < pipelines.size(); ++i) {
        if (pipelines[i] != next[i]) break;
    }

    Diff diff;
    diff.type          = pipelines[i] == Config::PipelineIds::None ? Diff::Add :
                         next[i] == Config::PipelineIds::None      ? Diff::Remove :
                                                                     Diff::Edit;
    diff.renderStageId = i;
    diff.pipelineId    = next[i];

    return diff;
}

void StagePipelines::applyDiff(const Diff& d) { pipelines[d.renderStageId] = d.pipelineId; }

void StagePipelines::addOrSetStagePipeline(std::uint32_t renderStageId, std::uint32_t pipelineId) {
    if (pipelines[renderStageId] != pipelineId) { next[renderStageId] = pipelineId; }
}

void StagePipelines::removeFromStage(std::uint32_t stageId) {
    if (pipelines[stageId] != Config::PipelineIds::None) {
        next[stageId] = Config::PipelineIds::None;
    }
}

void StagePipelines::prepareForNewScene() { pipelines.fill(Config::PipelineIds::None); }

std::uint32_t StagePipelines::getPipelineForRenderStage(std::uint32_t stage) const {
    return pipelines[stage];
}

} // namespace render
} // namespace bl

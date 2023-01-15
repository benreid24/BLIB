#include <BLIB/Render/Renderables/RenderPassMembership.hpp>

#include <BLIB/Render/Config.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{

RenderPassMembership::RenderPassMembership() {
    pipelines.fill(Config::PipelineIds::None);
    next.fill(Config::PipelineIds::None);
}

bool RenderPassMembership::hasDiff() const {
    for (unsigned int i = 0; i < pipelines.size(); ++i) {
        if (pipelines[i] != next[i]) return true;
    }
    return false;
}

RenderPassMembership::Diff RenderPassMembership::nextDiff() {
    unsigned int i = 0;
    for (; i < pipelines.size(); ++i) {
        if (pipelines[i] != next[i]) break;
    }

    Diff diff;
    diff.type         = pipelines[i] == Config::PipelineIds::None ? Diff::Add :
                        next[i] == Config::PipelineIds::None      ? Diff::Remove :
                                                                    Diff::Edit;
    diff.renderPassId = i;
    diff.pipelineId   = next[i];

    return diff;
}

void RenderPassMembership::applyDiff(const Diff& d) { pipelines[d.renderPassId] = d.pipelineId; }

void RenderPassMembership::addOrSetPassPipeline(std::uint32_t renderPassId,
                                                std::uint32_t pipelineId) {
    if (pipelines[renderPassId] != pipelineId) { next[renderPassId] = pipelineId; }
}

void RenderPassMembership::removeFromPass(std::uint32_t passId) {
    if (pipelines[passId] != Config::PipelineIds::None) {
        next[passId] = Config::PipelineIds::None;
    }
}

void RenderPassMembership::prepareForNewScene() { pipelines.fill(Config::PipelineIds::None); }

std::uint32_t RenderPassMembership::getPipelineForRenderPass(std::uint32_t pass) const {
    return pipelines[pass];
}

} // namespace render
} // namespace bl

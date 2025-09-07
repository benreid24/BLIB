#include <BLIB/Render/Graph/Tasks/AutoExposureAdjustTask.hpp>

#include <BLIB/Render/Config/ComputePipelineIds.hpp>
#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Descriptors/Builtin/InputAttachmentInstance.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
namespace
{
constexpr std::uint32_t WorkGroupSize = 32;
}
AutoExposureAdjustTask::AutoExposureAdjustTask()
: Task(rg::TaskIds::AutoExposureAdjustTask)
, renderer(nullptr)
, scene(nullptr) {
    assetTags.requiredInputs.emplace_back(rg::TaskInput(rg::AssetTags::RenderedSceneOutput));
    // TODO - side effect output & refactor graph construction
}

void AutoExposureAdjustTask::create(engine::Engine&, Renderer& r, Scene* s) {
    renderer = &r;
    scene    = s;
}

void AutoExposureAdjustTask::onGraphInit() {
    const auto sampler = renderer->samplerCache().noFilterEdgeClamped();
    FramebufferAsset* input =
        dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }

    vk::ComputePipeline* adjust =
        &renderer->computePipelineCache().getPipeline(cfg::ComputePipelineIds::AutoExposureAdjust);
    pipeline.init(adjust, scene->getDescriptorSets());
}

void AutoExposureAdjustTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    // buffer barrier handled by accumulate task

    // TODO - barrier to ensure not overwrite exposure until postfx done

    pipeline.bind(ctx, cfg::RenderPhases::PostProcess);
    pipeline.getComputePipeline()->exec(ctx.commandBuffer, 1);

    // TODO - pipeline barrier
}

void AutoExposureAdjustTask::update(float) {}

} // namespace rgi
} // namespace rc
} // namespace bl

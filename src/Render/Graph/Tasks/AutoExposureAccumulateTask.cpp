#include <BLIB/Render/Graph/Tasks/AutoExposureAccumulateTask.hpp>

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
AutoExposureAccumulateTask::AutoExposureAccumulateTask()
: Task(rg::TaskIds::AutoExposureAccumulateTask)
, renderer(nullptr)
, scene(nullptr) {
    assetTags.requiredInputs.emplace_back(rg::TaskInput(rg::AssetTags::RenderedSceneOutput));
    assetTags.outputs.emplace_back(rg::TaskOutput({rg::AssetTags::AutoExposureWorkBuffer},
                                                  {rg::TaskOutput::CreatedByTask},
                                                  {rg::TaskOutput::Exclusive}));
}

void AutoExposureAccumulateTask::create(engine::Engine&, Renderer& r, Scene* s) {
    renderer = &r;
    scene    = s;
}

void AutoExposureAccumulateTask::onGraphInit() {
    const auto sampler = renderer->samplerCache().noFilterEdgeClamped();
    input              = dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }

    vk::ComputePipeline* accum = &renderer->computePipelineCache().getPipeline(
        cfg::ComputePipelineIds::AutoExposureAccumulate);
    pipeline.init(accum, scene->getDescriptorSets());
    pipeline.getDescriptorSet<dsi::InputAttachmentInstance>(0)->initAttachments(
        input->getAttachmentSets(), sampler);
}

void AutoExposureAccumulateTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    // attachment sync handled by render pass
    // asset handles buffer barrier for read -> write

    const std::uint32_t w       = input->getAttachmentSets()[0]->getRenderExtent().width;
    const std::uint32_t h       = input->getAttachmentSets()[0]->getRenderExtent().height;
    const std::uint32_t xGroups = w / WorkGroupSize + (w % WorkGroupSize != 0 ? 1 : 0);
    const std::uint32_t yGroups = h / WorkGroupSize + (h % WorkGroupSize != 0 ? 1 : 0);

    pipeline.bind(ctx, cfg::RenderPhases::PostProcess);
    pipeline.getComputePipeline()->exec(ctx.commandBuffer, xGroups, yGroups);

    // asset handles buffer barrier for write -> read
}

void AutoExposureAccumulateTask::update(float) {}

} // namespace rgi
} // namespace rc
} // namespace bl

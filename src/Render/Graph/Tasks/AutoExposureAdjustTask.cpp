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
    assetTags.requiredInputs.emplace_back(rg::TaskInput(rg::AssetTags::AutoExposureWorkBuffer));
    assetTags.outputs.emplace_back(rg::TaskOutput({rg::AssetTags::AutoExposureOutput},
                                                  {rg::TaskOutput::CreatedByTask},
                                                  {rg::TaskOutput::Exclusive}));
}

void AutoExposureAdjustTask::create(const rg::InitContext& ctx) {
    renderer = &ctx.renderer;
    target   = &ctx.target;
    scene    = ctx.scene;
}

void AutoExposureAdjustTask::onGraphInit() {
    vk::ComputePipeline* adjust =
        &renderer->computePipelineCache().getPipeline(cfg::ComputePipelineIds::AutoExposureAdjust);
    pipeline.init(adjust, *target->getDescriptorSetCache(scene));
}

void AutoExposureAdjustTask::execute(const rg::ExecutionContext& ctx, rg::Asset*) {
    // buffer barrier handled by accumulate task

    pipeline.bind(ctx, cfg::RenderPhases::PostProcess);
    pipeline.getComputePipeline()->exec(ctx.commandBuffer, 1);

    // asset inserts barrier for read -> write next frame

    // block postfx read until done write
    renderer->getGlobalDescriptorData()
        .getDynamicSettingsBuffer()
        .getBuffer()
        .getCurrentFrameBuffer()
        .recordBarrier(ctx.commandBuffer,
                       VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                       VK_ACCESS_SHADER_WRITE_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                       VK_ACCESS_SHADER_READ_BIT);
}

void AutoExposureAdjustTask::update(float) {}

} // namespace rgi
} // namespace rc
} // namespace bl

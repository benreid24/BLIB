#include <BLIB/Render/Graph/Tasks/DeferredCompositeTask.hpp>

#include <BLIB/Render/Config/PipelineIds.hpp>
#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Config/SpecializationsLightVolumes.hpp>
#include <BLIB/Render/Descriptors/Builtin/InputAttachmentFactory.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/BloomAssets.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
DeferredCompositeTask::DeferredCompositeTask()
: Task(rg::TaskIds::DeferredCompositeTask)
, renderer(nullptr) {
    assetTags.outputs.emplace_back(
        rg::TaskOutput({rg::AssetTags::RenderedSceneOutput, rg::AssetTags::FinalFrameOutput},
                       {rg::TaskOutput::CreatedByTask, rg::TaskOutput::CreatedExternally},
                       {rg::TaskOutput::Shared},
                       rg::TaskOutput::First));
    assetTags.requiredInputs.emplace_back(rg::TaskInput(rg::AssetTags::GBuffer));
    assetTags.optionalInputs.emplace_back(rg::TaskInput(rg::AssetTags::SSAOBuffer));
}

void DeferredCompositeTask::create(engine::Engine&, Renderer& r, Scene* s) {
    renderer = &r;
    scene    = s;

    // fetch pipeline
    pipeline = &renderer->pipelineCache().getPipeline(cfg::PipelineIds::DeferredLightVolume);

    // create index buffer for the sun
    sunRectBuffer.create(r.vulkanState(), 4, 6);
    sunRectBuffer.indices()  = {0, 1, 3, 1, 2, 3};
    sunRectBuffer.vertices() = {prim::Vertex3D({-1.f, -1.f, 1.0f}, {0.f, 0.f}),
                                prim::Vertex3D({1.f, -1.f, 1.0f}, {1.f, 0.f}),
                                prim::Vertex3D({1.f, 1.f, 1.0f}, {1.f, 1.f}),
                                prim::Vertex3D({-1.f, 1.f, 1.0f}, {0.f, 1.f})};
    sunRectBuffer.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);

    // make unit sphere index buffer
    std::vector<prim::Vertex3D> sphereVertices;
    std::vector<std::uint32_t> sphereIndices;
    gfx::Sphere::makeSphere(1.f, 3, sphereVertices, sphereIndices);
    sphereBuffer.create(r.vulkanState(), std::move(sphereVertices), std::move(sphereIndices));
    sphereBuffer.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);
}

void DeferredCompositeTask::onGraphInit() {
    FramebufferAsset* input =
        dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }

    const auto sampler   = renderer->samplerCache().noFilterEdgeClamped();
    const auto setLayout = renderer->descriptorFactoryCache()
                               .getOrCreateFactory<dsi::InputAttachmentFactory<4>>()
                               ->getDescriptorLayout();

    sceneDescriptor = &scene->getDescriptorSet<dsi::Scene3DInstance>();

    gbufferDescriptor.emplace(renderer->vulkanState(), setLayout, 4, 0);
    gbufferDescriptor.value().initAttachments(input->getAttachmentSets(), sampler);
}

void DeferredCompositeTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    const auto layout    = pipeline->pipelineLayout().rawLayout();
    const auto& lighting = sceneDescriptor->getUniform();
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got bad output"); }

    // bind sun pipeline & all descriptors
    pipeline->bind(ctx.commandBuffer, fb->getRenderPassId(), 0);
    gbufferDescriptor->bind(ctx.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0);
    sceneDescriptor->bind(
        ctx.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, ctx.observerIndex);

    // render sun
    sunRectBuffer.bindAndDraw(ctx.commandBuffer);

    // bind unit sphere buffers once
    const auto draw = sphereBuffer.getDrawParameters();
    sphereBuffer.bind(ctx.commandBuffer);

    // spot lights with shadows
    if (lighting.nSpotShadows > 0) {
        pipeline->bind(ctx.commandBuffer,
                       fb->getRenderPassId(),
                       cfg::SpecializationsLightVolumes::SpotlightShadow);
        vkCmdDrawIndexed(ctx.commandBuffer,
                         draw.indexCount,
                         lighting.nSpotShadows,
                         draw.indexOffset,
                         draw.vertexOffset,
                         0);
    }

    // spot lights
    if (lighting.nSpotLights > 0) {
        pipeline->bind(
            ctx.commandBuffer, fb->getRenderPassId(), cfg::SpecializationsLightVolumes::Spotlight);
        vkCmdDrawIndexed(ctx.commandBuffer,
                         draw.indexCount,
                         lighting.nSpotLights,
                         draw.indexOffset,
                         draw.vertexOffset,
                         0);
    }

    // point lights with shadows
    if (lighting.nPointShadows > 0) {
        pipeline->bind(ctx.commandBuffer,
                       fb->getRenderPassId(),
                       cfg::SpecializationsLightVolumes::PointlightShadow);
        vkCmdDrawIndexed(ctx.commandBuffer,
                         draw.indexCount,
                         lighting.nPointShadows,
                         draw.indexOffset,
                         draw.vertexOffset,
                         0);
    }

    // point lights
    if (lighting.nPointLights > 0) {
        pipeline->bind(
            ctx.commandBuffer, fb->getRenderPassId(), cfg::SpecializationsLightVolumes::Pointlight);
        vkCmdDrawIndexed(ctx.commandBuffer,
                         draw.indexCount,
                         lighting.nPointLights,
                         draw.indexOffset,
                         draw.vertexOffset,
                         0);
    }
}

} // namespace rgi
} // namespace rc
} // namespace bl

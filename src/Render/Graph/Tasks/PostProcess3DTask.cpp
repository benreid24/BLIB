#include <BLIB/Render/Graph/Tasks/PostProcess3DTask.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/Assets/StandardTargetAsset.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
PostProcess3DTask::PostProcess3DTask()
: renderer(nullptr)
, output(nullptr) {
    assetTags.concreteOutputs.emplace_back(rg::AssetTags::FinalFrameOutput);
    assetTags.createdOutput = rg::AssetTags::PostFXOutput;
    assetTags.requiredInputs.emplace_back(rg::AssetTags::RenderedSceneOutput);
}

void PostProcess3DTask::create(engine::Engine&, Renderer& r, Scene* s) {
    renderer = &r;
    scene    = s;

    // fetch pipeline
    s->initPipelineInstance(Config::PipelineIds::PostProcess3D, pipeline);
    colorAttachmentSet = &s->getDescriptorSet<ds::ColorAttachmentInstance>();

    // create index buffer
    indexBuffer.create(r.vulkanState(), 4, 6);
    indexBuffer.indices()  = {0, 1, 3, 1, 2, 3};
    indexBuffer.vertices() = {prim::Vertex({-1.f, -1.f, 1.0f}, {0.f, 0.f}),
                              prim::Vertex({1.f, -1.f, 1.0f}, {1.f, 0.f}),
                              prim::Vertex({1.f, 1.f, 1.0f}, {1.f, 1.f}),
                              prim::Vertex({-1.f, 1.f, 1.0f}, {0.f, 1.f})};
    indexBuffer.queueTransfer(tfr::Transferable::SyncRequirement::Immediate);
}

void PostProcess3DTask::onGraphInit() {
    FramebufferAsset* input =
        dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }

    output = dynamic_cast<FramebufferAsset*>(&assets.output->asset.get());
    if (!output) { throw std::runtime_error("Got bad output"); }

    auto& set = scene->getDescriptorSet<ds::ColorAttachmentInstance>();
    set.initAttachments(
        &input->getFramebuffer(0), 0, renderer->vulkanState().samplerCache.filteredBorderClamped());
}

void PostProcess3DTask::execute(const rg::ExecutionContext& ctx) {
    output->beginRender(ctx.commandBuffer, true);

    scene::SceneRenderContext renderCtx(ctx.commandBuffer,
                                        ctx.observerIndex,
                                        output->getViewport(),
                                        RenderPhase::PostProcess,
                                        output->getRenderPassId(),
                                        ctx.renderingToRenderTexture);

    pipeline.bind(renderCtx);
    indexBuffer.bindAndDraw(ctx.commandBuffer);

    output->finishRender(ctx.commandBuffer);
}

void PostProcess3DTask::update(float) {}

} // namespace rgi
} // namespace rc
} // namespace bl

#include <BLIB/Render/Graph/Tasks/FadeEffectTask.hpp>

#include <BLIB/Render/Config/PipelineIds.hpp>
#include <BLIB/Render/Config/RenderPhases.hpp>
#include <BLIB/Render/Descriptors/Builtin/ColorAttachmentInstance.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Graph/TaskIds.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
FadeEffectTask::FadeEffectTask(float fadeTime, float start, float end)
: Task(rg::TaskIds::FadeEffectTask)
, renderer(nullptr) {
    assetTags.outputs.emplace_back(
        rg::TaskOutput({rg::AssetTags::PostFXOutput, rg::AssetTags::FinalFrameOutput},
                       {rg::TaskOutput::CreatedByTask, rg::TaskOutput::CreatedExternally},
                       {rg::TaskOutput::Exclusive, rg::TaskOutput::Shared}));
    assetTags.requiredInputs.emplace_back(rg::AssetTags::RenderedSceneOutput,
                                          rg::TaskInput::Exclusive);

    fade(fadeTime, start, end);
}

FadeEffectTask::~FadeEffectTask() {}

void FadeEffectTask::fadeTo(float fadeTime, float factorEnd) {
    fadeEnd   = factorEnd;
    fadeSpeed = (fadeEnd - factor) / fadeTime;
}

void FadeEffectTask::fade(float fadeTime, float fadeStart, float factorEnd) {
    factor    = fadeStart;
    fadeEnd   = factorEnd;
    fadeSpeed = (fadeEnd - factor) / fadeTime;
}

void FadeEffectTask::create(engine::Engine&, Renderer& r, Scene* s) {
    renderer = &r;
    scene    = s;

    // fetch pipeline and descriptor set
    s->initPipelineInstance(cfg::PipelineIds::FadeEffect, pipeline);
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

void FadeEffectTask::onGraphInit() {
    FramebufferAsset* input =
        dynamic_cast<FramebufferAsset*>(&assets.requiredInputs[0]->asset.get());
    if (!input) { throw std::runtime_error("Got bad input"); }

    auto& set = scene->getDescriptorSet<ds::ColorAttachmentInstance>();
    set.initAttachments(
        input->getAttachmentSets(), 0, renderer->samplerCache().noFilterEdgeClamped());
}

void FadeEffectTask::execute(const rg::ExecutionContext& ctx, rg::Asset* output) {
    FramebufferAsset* fb = dynamic_cast<FramebufferAsset*>(output);
    if (!fb) { throw std::runtime_error("Got bad output"); }

    fb->beginRender(ctx.commandBuffer, true);

    scene::SceneRenderContext renderCtx(ctx.commandBuffer,
                                        ctx.observerIndex,
                                        fb->getViewport(),
                                        cfg::RenderPhases::PostProcess,
                                        fb->getRenderPassId(),
                                        ctx.renderingToRenderTexture);

    pipeline.bind(renderCtx);
    vkCmdPushConstants(ctx.commandBuffer,
                       pipeline.getPipeline().pipelineLayout().rawLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       0,
                       sizeof(float),
                       &factor);
    indexBuffer.bindAndDraw(ctx.commandBuffer);

    fb->finishRender(ctx.commandBuffer);
}

void FadeEffectTask::update(float dt) {
    if (factor < fadeEnd) {
        factor += fadeSpeed * dt;
        if (factor >= fadeEnd) {
            fadeSpeed = 0.f;
            factor    = fadeEnd;
        }
    }
    else if (factor > fadeEnd) {
        factor += fadeSpeed * dt;
        if (factor <= fadeEnd) {
            fadeSpeed = 0.f;
            factor    = fadeEnd;
        }
    }
}

} // namespace rgi
} // namespace rc
} // namespace bl

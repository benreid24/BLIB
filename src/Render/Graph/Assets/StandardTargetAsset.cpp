#include <BLIB/Render/Graph/Assets/StandardTargetAsset.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
StandardTargetAsset::StandardTargetAsset()
: FramebufferAsset(rg::AssetTags::RenderedSceneOutput,
                   Config::RenderPassIds::StandardAttachmentDefault, cachedViewport, cachedScissor,
                   nullptr, 2)
, renderer(nullptr)
, cachedViewport{}
, cachedScissor{} {
    cachedViewport.minDepth = 0.f;
    cachedViewport.maxDepth = 1.f;
    cachedScissor.offset.x  = 0;
    cachedScissor.offset.y  = 0;
    cachedViewport.x        = 0.f;
    cachedViewport.y        = 0.f;
}

void StandardTargetAsset::doCreate(engine::Engine&, Renderer& r, Observer* o) {
    renderer = &r;
    observer = o;
    images.emptyInit(r.vulkanState());
    framebuffers.emptyInit(r.vulkanState());
    renderPass = &renderer->renderPassCache().getRenderPass(renderPassId);
    onResize(o->getRegionSize());
}

void StandardTargetAsset::doPrepareForInput(const rg::ExecutionContext&) {
    // noop, handled by render pass
}

void StandardTargetAsset::doPrepareForOutput(const rg::ExecutionContext&) {
    // noop, handled by render pass
}

void StandardTargetAsset::onResize(glm::u32vec2 newSize) {
    clearColors                 = observer->getClearColors();
    cachedScissor.extent.width  = newSize.x;
    cachedScissor.extent.height = newSize.y;
    cachedViewport.width        = static_cast<float>(newSize.x);
    cachedViewport.height       = static_cast<float>(newSize.y);

    if (renderer) {
        images.init(renderer->vulkanState(), [this, newSize](vk::StandardAttachmentBuffers& image) {
            image.create(renderer->vulkanState(), {newSize.x, newSize.y});
        });
        unsigned int i = 0;
        framebuffers.init(renderer->vulkanState(), [this, &i](vk::Framebuffer& fb) {
            fb.create(
                renderer->vulkanState(), renderPass->rawPass(), images.getRaw(i).attachmentSet());
            ++i;
        });
    }
}

vk::Framebuffer& StandardTargetAsset::currentFramebuffer() { return framebuffers.current(); }

} // namespace rgi
} // namespace rc
} // namespace bl

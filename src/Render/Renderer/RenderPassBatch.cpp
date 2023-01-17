#include <BLIB/Render/Renderer/RenderPassBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
RenderPassBatch::RenderPassBatch(Renderer& renderer, std::uint32_t id)
: renderer(renderer) {
    renderPass = renderer.renderPassCache().getRenderPass(id).rawPass();
    batches.reserve(16);

    unsigned int i = 0;
    framebuffers.init(renderer.vulkanState().swapchain, [this, &renderer, &i](Framebuffer& fb) {
        fb.create(renderer.vulkanState(),
                  renderPass,
                  renderer.vulkanState().swapchain.swapFrameAtIndex(i));
        ++i;
    });
}

void RenderPassBatch::addObject(const Object::Handle& obj, std::uint32_t pid) {
    for (PipelineBatch& p : batches) {
        if (p.pipelineId == pid) {
            p.addObject(obj);
            return;
        }
    }
    batches.emplace_back(renderer, pid, false);
    batches.back().addObject(obj);
}

void RenderPassBatch::changePipeline(const Object::Handle& obj, std::uint32_t og,
                                     std::uint32_t pid) {
    removeObject(obj, og);
    addObject(obj, pid);
}

void RenderPassBatch::removeObject(const Object::Handle& obj, std::uint32_t pid) {
    for (PipelineBatch& p : batches) {
        if (p.pipelineId == pid) {
            p.removeObject(obj);
            return;
        }
    }
}

void RenderPassBatch::recordRenderCommands(VkCommandBuffer cb, const RenderFrame& target,
                                           VkClearValue* clearColors,
                                           std::uint32_t clearColorCount) {
    framebuffers.current().recreateIfChanged(target);

    framebuffers.current().beginRender(cb, clearColors, clearColorCount);
    for (PipelineBatch& p : batches) { p.recordRenderCommands(cb); }
    framebuffers.current().finishRender(cb);
}

} // namespace render
} // namespace bl

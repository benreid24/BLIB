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
    RenderPass* pass = renderer.renderPassCache().getRenderPass(id);
    if (!pass) {
        BL_LOG_CRITICAL << "Failed to find render pass with id: " << id;
        throw std::runtime_error("Failed to find render pass");
    }
    renderPass = pass->rawPass();
    batches.reserve(16);
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

void RenderPassBatch::recordRenderCommands(VkCommandBuffer cb, VkRenderPassBeginInfo& beginInfo) {
    beginInfo.renderPass = renderPass;

    // TODO - VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS for multithreading
    vkCmdBeginRenderPass(cb, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    // TODO - subpasses? viewport and scissor from camera
    // vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);
    // vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);
    for (PipelineBatch& p : batches) { p.recordRenderCommands(cb); }
    vkCmdEndRenderPass(cb);
}

} // namespace render
} // namespace bl

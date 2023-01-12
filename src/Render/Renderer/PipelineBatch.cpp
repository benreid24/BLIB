#include <BLIB/Render/Renderer/PipelineBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderables/Renderable.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
PipelineBatch::PipelineBatch(Renderer& renderer, std::uint32_t pid, bool po)
: pipelineId(pid)
, preserveObjectOrder(po)
, pipeline(renderer.pipelineCache().getPipeline(pid)) {
    objects.reserve(128);
}

void PipelineBatch::recordRenderCommands(VkCommandBuffer cb) {
    pipeline.bindPipelineAndDescriptors(cb);
    VkPipelineLayout layout = pipeline.pipelineLayout();

    VkBuffer prevVB = nullptr;
    VkBuffer prevIB = nullptr;

    for (Object::Handle& handle : objects) {
        Object& object = *handle;
        if (object.hidden) continue;

        // TODO - move this to scene render start
        if (object.flags.isDirty()) {
            if (object.flags.isPCDirty()) { object.owner->syncPC(); }
            // TODO - other flags
            object.flags.reset();
        }

        if (prevVB != object.drawParams.vertexBuffer) {
            prevVB = object.drawParams.vertexBuffer;

            VkBuffer vertexBuffers[] = {object.drawParams.vertexBuffer};
            VkDeviceSize offsets[]   = {0};
            vkCmdBindVertexBuffers(cb, 0, 1, vertexBuffers, offsets);
        }
        if (prevIB != object.drawParams.indexBuffer) {
            prevIB = object.drawParams.indexBuffer;
            vkCmdBindIndexBuffer(cb, object.drawParams.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }

        vkCmdPushConstants(
            cb, layout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(PushConstants), &object.frameData);
        vkCmdDrawIndexed(cb,
                         object.drawParams.indexCount,
                         1,
                         object.drawParams.indexOffset,
                         object.drawParams.vertexOffset,
                         0);
    }
}

void PipelineBatch::addObject(const Object::Handle& object) { objects.emplace_back(object); }

void PipelineBatch::removeObject(const Object::Handle& object) {
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it->id() == object.id()) {
            if (preserveObjectOrder || objects.size() == 1) { objects.erase(it); }
            else {
                *it = objects.back();
                objects.pop_back();
            }
            return;
        }
    }
}

} // namespace render
} // namespace bl

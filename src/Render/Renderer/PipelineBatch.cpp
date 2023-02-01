#include <BLIB/Render/Renderer/PipelineBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderables/Renderable.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
PipelineBatch::PipelineBatch(Renderer& renderer, std::uint32_t pid)
: pipelineId(pid)
, pipeline(renderer.pipelineCache().getPipeline(pid)) {
    objects.reserve(128);
}

void PipelineBatch::recordRenderCommands(const SceneRenderContext& context) {
    pipeline.bindPipelineAndDescriptors(context.commandBuffer);
    VkPipelineLayout layout = pipeline.pipelineLayout();

    VkBuffer prevVB = nullptr;
    VkBuffer prevIB = nullptr;

    for (SceneObject::Handle& handle : objects) {
        SceneObject& object = *handle;
        if (object.hidden) continue;

        if (prevVB != object.drawParams.vertexBuffer) {
            prevVB = object.drawParams.vertexBuffer;

            VkBuffer vertexBuffers[] = {object.drawParams.vertexBuffer};
            VkDeviceSize offsets[]   = {0};
            vkCmdBindVertexBuffers(context.commandBuffer, 0, 1, vertexBuffers, offsets);
        }
        if (prevIB != object.drawParams.indexBuffer) {
            prevIB = object.drawParams.indexBuffer;
            vkCmdBindIndexBuffer(
                context.commandBuffer, object.drawParams.indexBuffer, 0, IndexBuffer::IndexType);
        }

        PushConstants pc = object.frameData;
        pc.transform     = context.projView * pc.transform;
        vkCmdPushConstants(context.commandBuffer,
                           layout,
                           VK_SHADER_STAGE_ALL_GRAPHICS,
                           0,
                           sizeof(PushConstants),
                           &pc);
        vkCmdDrawIndexed(context.commandBuffer,
                         object.drawParams.indexCount,
                         1,
                         object.drawParams.indexOffset,
                         object.drawParams.vertexOffset,
                         0);
    }
}

void PipelineBatch::addObject(const SceneObject::Handle& object) { objects.emplace_back(object); }

void PipelineBatch::removeObject(const SceneObject::Handle& object) {
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it->id() == object.id()) {
            if (pipeline.preserveObjectOrder() || objects.size() == 1) { objects.erase(it); }
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

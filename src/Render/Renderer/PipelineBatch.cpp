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

void PipelineBatch::recordRenderCommands(VkCommandBuffer cb) {
    pipeline.bindPipelineAndDescriptors(cb);
    VkPipelineLayout layout = pipeline.pipelineLayout();

    VkBuffer prevVB = nullptr;
    VkBuffer prevIB = nullptr;

    // TODO - make actual camera
    glm::mat4 view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::mat4(1.f);
    //    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1920.f / 1080.f, 0.1f, 10.0f);
    glm::mat4 proj = glm::ortho(-1.f, 1.f, -1.f, 1.f);
    proj[1][1] *= -1.f;

    for (SceneObject::Handle& handle : objects) {
        SceneObject& object = *handle;
        if (object.hidden) continue;

        if (object.flags.isDirty()) {
            if (object.flags.isPCDirty()) { object.owner->syncPC(); }
            if (object.flags.isDrawParamsDirty()) { object.owner->syncDrawParams(); }
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

        PushConstants pc = object.frameData;
        pc.transform = proj * view * pc.transform; // TODO - put camera params in descriptor set?
        vkCmdPushConstants(cb, layout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(PushConstants), &pc);
        vkCmdDrawIndexed(cb,
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

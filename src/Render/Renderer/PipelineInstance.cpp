#include <BLIB/Render/Renderer/PipelineInstance.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderables/Renderable.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
PipelineInstance::PipelineInstance(Renderer& renderer, std::uint32_t pid, bool po)
: vulkanState(renderer.vulkanState())
, preserveObjectOrder(po)
, pipeline(renderer.pipelineCache().getPipeline(pid)) {
    if (!pipeline) {
        BL_LOG_CRITICAL << "Failed to find pipeline: " << pid;
        throw std::runtime_error("Failed to find pipeline by id during instance creation");
    }
    objects.reserve(128);
}

PipelineInstance::~PipelineInstance() {
    // any cleanup?
}

void PipelineInstance::recordRenderCommands(VkCommandBuffer cb) {
    pipeline->bindPipelineAndDescriptors(cb);
    VkPipelineLayout layout = pipeline->pipelineLayout();

    VkBuffer prevVB = nullptr;
    VkBuffer prevIB = nullptr;

    std::unique_lock lock(mutex);
    for (Object& object : objects) {
        if (object.hidden) continue;

        if (object.flags.isDirty()) {
            if (object.flags.isPCDirty()) { object.owner->syncPC(); }
            object.flags.reset();
        }

        if (prevVB != object.vertexBuffer) {
            prevVB = object.vertexBuffer;

            VkBuffer vertexBuffers[] = {object.vertexBuffer};
            VkDeviceSize offsets[]   = {0};
            vkCmdBindVertexBuffers(cb, 0, 1, vertexBuffers, offsets);
        }
        if (prevIB != object.indexBuffer) {
            prevIB = object.indexBuffer;
            vkCmdBindIndexBuffer(cb, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }

        vkCmdPushConstants(
            cb, layout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, sizeof(PushConstants), &object.frameData);
        vkCmdDrawIndexed(cb, object.indexCount, 1, object.indexOffset, object.vertexOffset, 0);
    }
}

Object* PipelineInstance::createAndAddObject(Renderable* owner) {
    std::unique_lock lock(mutex);

    const bool reallocated = objects.size() == objects.capacity();

    objects.emplace_back();
    objects.back().owner = owner;
    objects.back().flags.markPCDirty();

    if (reallocated) {
        for (Object& object : objects) { object.owner->object = &object; }
    }

    return &objects.back();
}

void PipelineInstance::removeObject(Object* obj) {
    std::unique_lock lock(mutex);

#ifdef BLIB_DEBUG
    if (obj < objects.data()) {
        BL_LOG_ERROR << "Object address is below buffer address";
        return;
    }
#endif
    const std::size_t i = obj - objects.data();
#ifdef BLIB_DEBUG
    if (i >= objects.size()) {
        BL_LOG_ERROR << "Object index to be removed is out of bounds";
        return;
    }
#endif

    if (!preserveObjectOrder && objects.size() > 1) {
        objects[i]                  = objects.back();
        objects.back().owner->owner = nullptr; // prevent removal
        objects.pop_back();
        objects[i].owner->object = &objects[i];
    }
    else {
        objects.erase(objects.begin() + i);
        for (auto it = objects.begin() + i; it != objects.end(); ++it) {
            it->owner->object = &(*it);
        }
    }
}

} // namespace render
} // namespace bl

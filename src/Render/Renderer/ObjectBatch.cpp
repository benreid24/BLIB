#include <BLIB/Render/Renderer/ObjectBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
ObjectBatch::ObjectBatch(Renderer& renderer)
: renderer(renderer) {
    batches.reserve(16);
}

void ObjectBatch::addObject(const SceneObject::Handle& obj, std::uint32_t pid) {
    for (PipelineBatch& p : batches) {
        if (p.pipelineId == pid) {
            p.addObject(obj);
            return;
        }
    }
    batches.emplace_back(renderer, pid);
    batches.back().addObject(obj);
}

void ObjectBatch::changePipeline(const SceneObject::Handle& obj, std::uint32_t og,
                                 std::uint32_t pid) {
    removeObject(obj, og);
    addObject(obj, pid);
}

void ObjectBatch::removeObject(const SceneObject::Handle& obj, std::uint32_t pid) {
    for (PipelineBatch& p : batches) {
        if (p.pipelineId == pid) {
            p.removeObject(obj);
            return;
        }
    }
}

void ObjectBatch::recordRenderCommands(VkCommandBuffer cb) {
    for (PipelineBatch& p : batches) { p.recordRenderCommands(cb); }
}

} // namespace render
} // namespace bl

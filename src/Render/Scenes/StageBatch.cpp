#include <BLIB/Render/Scenes/StageBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
StageBatch::StageBatch(Renderer& renderer)
: renderer(renderer) {
    batches.reserve(16);
}

void StageBatch::addObject(const SceneObject::Handle& obj, std::uint32_t pid) {
    for (PipelineBatch& p : batches) {
        if (p.pipelineId == pid) {
            p.addObject(obj);
            return;
        }
    }
    batches.emplace_back(renderer, pid);
    batches.back().addObject(obj);
}

void StageBatch::changePipeline(const SceneObject::Handle& obj, std::uint32_t og,
                                std::uint32_t pid) {
    removeObject(obj, og);
    addObject(obj, pid);
}

void StageBatch::removeObject(const SceneObject::Handle& obj, std::uint32_t pid) {
    for (PipelineBatch& p : batches) {
        if (p.pipelineId == pid) {
            p.removeObject(obj);
            return;
        }
    }
}

void StageBatch::recordRenderCommands(const SceneRenderContext& context) {
    for (PipelineBatch& p : batches) { p.recordRenderCommands(context); }
}

} // namespace render
} // namespace bl

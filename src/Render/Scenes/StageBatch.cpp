#include <BLIB/Render/Scenes/StageBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
namespace scene
{
StageBatch::StageBatch(Renderer& renderer, std::uint32_t maxObjects,
                       ds::DescriptorSetInstanceCache& descriptorCache)
: maxObjects(maxObjects)
, renderer(renderer)
, descriptorCache(descriptorCache) {
    batches.reserve(16);
}

bool StageBatch::addObject(SceneObject* object, std::uint32_t pipelineId, ecs::Entity entity,
                           UpdateSpeed updateFreq) {
    for (PipelineBatch& p : batches) {
        if (p.pipelineId == pipelineId) { return p.addObject(object, entity, updateFreq); }
    }

    // not found, create new pipeline batch
    batches.emplace_back(renderer, maxObjects, descriptorCache, pipelineId);
    return batches.back().addObject(object, entity, updateFreq);
}

void StageBatch::removeObject(SceneObject* object, std::uint32_t pipelineId, ecs::Entity entity) {
    for (PipelineBatch& p : batches) {
        if (p.pipelineId == pipelineId) {
            p.removeObject(object, entity);
            return;
        }
    }
}

void StageBatch::recordRenderCommands(SceneRenderContext& context) {
    for (PipelineBatch& p : batches) { p.recordRenderCommands(context); }
}

} // namespace scene
} // namespace render
} // namespace bl

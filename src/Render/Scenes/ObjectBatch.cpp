#include <BLIB/Render/Scenes/ObjectBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <stdexcept>

namespace bl
{
namespace gfx
{
namespace scene
{
ObjectBatch::ObjectBatch(Renderer& renderer, std::uint32_t maxObjects,
                         ds::DescriptorSetInstanceCache& descriptorCache)
: maxObjects(maxObjects)
, renderer(renderer)
, descriptorCache(descriptorCache) {
    batches.reserve(16);
}

bool ObjectBatch::addObject(SceneObject* object, std::uint32_t pipelineId, ecs::Entity entity,
                            UpdateSpeed updateFreq) {
    vk::Pipeline& pipeline = renderer.pipelineCache().getPipeline(pipelineId);
    for (LayoutBatch& p : batches) {
        if (p.getLayout() == pipeline.pipelineLayout().rawLayout()) {
            return p.addObject(object, entity, pipeline, updateFreq);
        }
    }

    // not found, create new layout batch
    batches.emplace_back(renderer, maxObjects, descriptorCache, pipeline.pipelineLayout());
    return batches.back().addObject(object, entity, pipeline, updateFreq);
}

void ObjectBatch::removeObject(SceneObject* object, std::uint32_t pipelineId, ecs::Entity entity) {
    vk::Pipeline& pipeline = renderer.pipelineCache().getPipeline(pipelineId);
    for (LayoutBatch& p : batches) {
        if (p.getLayout() == pipeline.pipelineLayout().rawLayout()) {
            p.removeObject(object, entity, pipelineId);
            return;
        }
    }
}

void ObjectBatch::recordRenderCommands(SceneRenderContext& context) {
    for (LayoutBatch& p : batches) { p.recordRenderCommands(context); }
}

} // namespace scene
} // namespace gfx
} // namespace bl

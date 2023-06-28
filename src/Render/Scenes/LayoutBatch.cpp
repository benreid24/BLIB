#include <BLIB/Render/Scenes/LayoutBatch.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace scene
{
LayoutBatch::LayoutBatch(Renderer& renderer, std::uint32_t maxObjects,
                         ds::DescriptorSetInstanceCache& descriptorCache,
                         const vk::PipelineLayout& layout)
: renderer(renderer)
, maxObjects(maxObjects)
, layout(layout)
, descriptorCount(layout.initDescriptorSets(descriptorCache, descriptors.data()))
, perObjStart(descriptorCount) {
    batches.reserve(8);
    for (std::uint8_t i = 0; i < descriptorCount; ++i) {
        if (descriptors[i]->isPerObject()) {
            perObjStart = i;
            break;
        }
    }
}

void LayoutBatch::recordRenderCommands(SceneRenderContext& ctx) {
    ctx.bindDescriptors(layout.rawLayout(), descriptors.data(), descriptorCount);
    for (PipelineBatch& batch : batches) { batch.recordRenderCommands(ctx); }
}

bool LayoutBatch::addObject(SceneObject* object, ecs::Entity entity, vk::Pipeline& pipeline,
                            UpdateSpeed updateFreq) {
    for (std::uint8_t i = 0; i < descriptorCount; ++i) {
        if (!descriptors[i]->allocateObject(object->sceneId, entity, updateFreq)) {
            for (std::int8_t j = i - 1; j >= 0; --j) {
                descriptors[j]->releaseObject(object->sceneId, entity);
            }
            return false;
        }
    }

    for (PipelineBatch& batch : batches) {
        if (batch.matches(pipeline)) {
            batch.addObject(object);
            return true;
        }
    }

    batches.emplace_back(
        maxObjects, pipeline, &descriptors[perObjStart], descriptorCount - perObjStart);
    batches.back().addObject(object);
    return true;
}

void LayoutBatch::removeObject(SceneObject* object, ecs::Entity entity, std::uint32_t pid) {
    for (std::uint8_t i = 0; i < descriptorCount; ++i) {
        descriptors[i]->releaseObject(object->sceneId, entity);
    }

    vk::Pipeline& pipeline = renderer.pipelineCache().getPipeline(pid);
    for (PipelineBatch& batch : batches) {
        if (batch.matches(pipeline)) {
            batch.removeObject(object);
            return;
        }
    }
}

} // namespace scene
} // namespace gfx
} // namespace bl

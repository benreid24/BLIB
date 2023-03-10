#include <BLIB/Render/Scenes/PipelineBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderables/Renderable.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
PipelineBatch::PipelineBatch(Renderer& renderer, std::uint32_t maxObjects,
                             ds::DescriptorSetInstanceCache& descriptorCache, std::uint32_t pid)
: pipelineId(pid)
, pipeline(renderer.pipelineCache().getPipeline(pid)) {
    pipeline.createDescriptorSets(descriptorCache, descriptors);
    objects.reserve(maxObjects);
}

void PipelineBatch::recordRenderCommands(SceneRenderContext& context) {
    const VkPipelineLayout pipelineLayout = pipeline.pipelineLayout();
    context.bindPipeline(pipeline.rawPipeline());
    context.bindDescriptors(pipelineLayout, descriptors);

    for (SceneObject* object : objects) {
        if (object->hidden) continue;
        context.renderObject(pipelineLayout, *object);
    }
}

void PipelineBatch::addObject(SceneObject* object, ecs::Entity entity,
                              SceneObject::UpdateSpeed updateFreq) {
    objects.emplace_back(object);
    for (ds::DescriptorSetInstance* set : descriptors) {
        set->allocateObject(object->sceneId, entity, updateFreq);
    }
}

void PipelineBatch::removeObject(SceneObject* object, ecs::Entity entity) {
    for (ds::DescriptorSetInstance* set : descriptors) {
        set->releaseObject(object->sceneId, entity);
    }

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (*it == object) {
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

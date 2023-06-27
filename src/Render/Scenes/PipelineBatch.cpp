#include <BLIB/Render/Scenes/PipelineBatch.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

namespace bl
{
namespace gfx
{
namespace scene
{
PipelineBatch::PipelineBatch(Renderer& renderer, std::uint32_t maxObjects,
                             ds::DescriptorSetInstanceCache& descriptorCache, std::uint32_t pid)
: pipelineId(pid)
, pipeline(renderer.pipelineCache().getPipeline(pid)) {
    pipeline.pipelineLayout().createDescriptorSets(descriptorCache, descriptors);
    objects.reserve(maxObjects);
}

void PipelineBatch::recordRenderCommands(SceneRenderContext& context) {
    const VkPipelineLayout pipelineLayout = pipeline.pipelineLayout().rawLayout();
    context.bindPipeline(pipeline);
    context.bindDescriptors(pipelineLayout, descriptors.data(), descriptors.size());

    for (SceneObject* object : objects) {
        if (object->hidden) continue;
        context.renderObject(pipelineLayout, *object);
    }
}

bool PipelineBatch::addObject(SceneObject* object, ecs::Entity entity, UpdateSpeed updateFreq) {
    objects.emplace_back(object);
    for (ds::DescriptorSetInstance* set : descriptors) {
        if (!set->allocateObject(object->sceneId, entity, updateFreq)) { return false; }
    }
    return true;
}

void PipelineBatch::removeObject(SceneObject* object, ecs::Entity entity) {
    for (ds::DescriptorSetInstance* set : descriptors) {
        set->releaseObject(object->sceneId, entity);
    }

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (*it == object) {
            if (objects.size() == 1) { objects.erase(it); }
            else {
                *it = objects.back();
                objects.pop_back();
            }
            return;
        }
    }
}

} // namespace scene
} // namespace gfx
} // namespace bl

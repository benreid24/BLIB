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
PipelineBatch::PipelineBatch(std::uint32_t maxObjects, vk::Pipeline& pipeline,
                             ds::DescriptorSetInstance** descriptors, std::uint8_t descriptorCount)
: pipeline(pipeline)
, descriptors(descriptors)
, descriptorCount(descriptorCount) {
    objects.reserve(maxObjects);
}

void PipelineBatch::recordRenderCommands(SceneRenderContext& context) {
    const VkPipelineLayout pipelineLayout = pipeline.pipelineLayout().rawLayout();
    context.bindPipeline(pipeline);

    for (SceneObject* object : objects) {
        if (object->hidden) continue;
        // TODO - bind per-object here after updating Overlay
        context.renderObject(pipelineLayout, *object);
    }
}

void PipelineBatch::addObject(SceneObject* object) { objects.emplace_back(object); }

void PipelineBatch::removeObject(SceneObject* object) {
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

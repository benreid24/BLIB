#include <BLIB/Render/Vulkan/PipelineInstance.hpp>

#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
PipelineInstance::PipelineInstance()
: pipeline(nullptr) {}

void PipelineInstance::init(Pipeline* p, ds::DescriptorSetInstanceCache& cache) {
    pipeline = p;

    descriptorSets.resize(pipeline->pipelineLayout().getDescriptorSetCount());
    pipeline->pipelineLayout().initDescriptorSets(cache, descriptorSets.data());
}

void PipelineInstance::bind(scene::SceneRenderContext& ctx, UpdateSpeed updateFreq) {
    pipeline->bind(ctx.getCommandBuffer(), ctx.currentRenderPass());
    for (unsigned int i = 0; i < descriptorSets.size(); ++i) {
        descriptorSets[i]->bindForPipeline(
            ctx, pipeline->pipelineLayout().rawLayout(), i, updateFreq);
    }
}

} // namespace vk
} // namespace rc
} // namespace bl

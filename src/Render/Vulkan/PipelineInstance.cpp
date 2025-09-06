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
: state(Uninitialized)
, gfxPipeline(nullptr) {}

void PipelineInstance::init(Pipeline* p, ds::DescriptorSetInstanceCache& cache) {
    state       = Graphics;
    gfxPipeline = p;

    descriptorSets.resize(p->pipelineLayout().getDescriptorSetCount());
    p->pipelineLayout().initDescriptorSets(cache, descriptorSets.data());
}

void PipelineInstance::init(ComputePipeline* p, ds::DescriptorSetInstanceCache& cache) {
    state           = Compute;
    computePipeline = p;

    descriptorSets.resize(p->pipelineLayout().getDescriptorSetCount());
    p->pipelineLayout().initDescriptorSets(cache, descriptorSets.data());
}

void PipelineInstance::bind(scene::SceneRenderContext& ctx, std::uint32_t specialization,
                            UpdateSpeed updateFreq) {
    switch (state) {
    case Uninitialized:
        BL_LOG_ERROR << "Attempted to bind uninitialized pipeline instance";
        break;
    case Graphics:
        gfxPipeline->bind(ctx.getCommandBuffer(), ctx.currentRenderPass(), specialization);
        break;
    case Compute:
        computePipeline->bind(ctx.getCommandBuffer());
        break;
    }

    VkPipelineLayout layout = getPipelineLayout().rawLayout();
    for (unsigned int i = 0; i < descriptorSets.size(); ++i) {
        descriptorSets[i]->bindForPipeline(ctx, layout, i, updateFreq);
    }
}

const PipelineLayout& PipelineInstance::getPipelineLayout() const {
    switch (state) {
    case Graphics:
        return gfxPipeline->pipelineLayout();
    case Compute:
        return computePipeline->pipelineLayout();
    case Uninitialized:
    default:
        throw std::runtime_error("Attempted to get layout of uninitialized pipeline instance");
    }
}

} // namespace vk
} // namespace rc
} // namespace bl

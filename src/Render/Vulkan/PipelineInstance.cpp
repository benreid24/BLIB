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
    initDescriptors(cache, p->pipelineLayout());
}

void PipelineInstance::init(ComputePipeline* p, ds::DescriptorSetInstanceCache& cache) {
    state           = Compute;
    computePipeline = p;
    initDescriptors(cache, p->pipelineLayout());
}

void PipelineInstance::initDescriptors(ds::DescriptorSetInstanceCache& cache,
                                       const PipelineLayout& layout) {
    ownedSets.clear();
    descriptorSets.resize(layout.getDescriptorSetCount());
    layout.initDescriptorSets(cache, descriptorSets.data());

    for (unsigned int i = 0; i < descriptorSets.size(); ++i) {
        if (!descriptorSets[i]) {
            if (layout.getDescriptorSetFactory(i)->isAutoConstructable()) {
                ownedSets.emplace_back(layout.getDescriptorSetFactory(i)->createDescriptorSet());
                descriptorSets[i] = ownedSets.back().get();
            }
            else { BL_LOG_ERROR << "Missing descriptor set " << i << " in pipeline instance"; }
        }
    }
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

void PipelineInstance::bind(const rg::ExecutionContext& ctx, RenderPhase renderPhase,
                            std::uint32_t renderPassId, const VkViewport& viewport,
                            std::uint32_t specialization, UpdateSpeed updateFreq) {
    scene::SceneRenderContext sceneCtx(ctx.commandBuffer,
                                       ctx.observerIndex,
                                       viewport,
                                       renderPhase,
                                       renderPassId,
                                       ctx.renderingToRenderTexture);
    bind(sceneCtx, specialization, updateFreq);
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

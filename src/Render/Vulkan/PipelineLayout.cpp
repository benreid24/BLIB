#include <BLIB/Render/Vulkan/PipelineLayout.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
PipelineLayout::PipelineLayout(Renderer& renderer, LayoutParams&& params)
: renderer(renderer)
, dsCount(params.dsCount) {
    // setup descriptors
    std::array<VkDescriptorSetLayout, 4> descriptorLayouts;
    for (unsigned int i = 0; i < params.dsCount; ++i) {
        descriptorSets[i] = renderer.descriptorFactoryCache().getOrAddFactory(
            params.descriptorSets[i].factoryType, std::move(params.descriptorSets[i].factory));
        descriptorLayouts[i] = descriptorSets[i]->getDescriptorLayout();
    }

    // create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = params.dsCount;
    pipelineLayoutInfo.pSetLayouts            = descriptorLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = params.pcCount;
    pipelineLayoutInfo.pPushConstantRanges    = params.pushConstants.data();
    if (vkCreatePipelineLayout(
            renderer.vulkanState().device, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(renderer.vulkanState().device, layout, nullptr);
}

void PipelineLayout::createDescriptorSets(
    ds::DescriptorSetInstanceCache& cache,
    std::vector<ds::DescriptorSetInstance*>& descriptors) const {
    descriptors.resize(dsCount);
    initDescriptorSets(cache, descriptors.data());
}

std::uint32_t PipelineLayout::initDescriptorSets(ds::DescriptorSetInstanceCache& cache,
                                                 ds::DescriptorSetInstance** sets) const {
    for (unsigned int i = 0; i < dsCount; ++i) {
        sets[i] = cache.getDescriptorSet(descriptorSets[i]);
    }
    return dsCount;
}

} // namespace vk
} // namespace gfx
} // namespace bl

#include <BLIB/Render/Vulkan/PipelineLayout.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
PipelineLayout::PipelineLayout(Renderer& renderer, LayoutParams&& params)
: renderer(renderer) {
    // setup descriptors
    std::array<VkDescriptorSetLayout, cfg::Limits::MaxDescriptorSets> descriptorLayouts;
    for (unsigned int i = 0; i < params.descriptorSets.size(); ++i) {
        descriptorSets.emplace_back(renderer.descriptorFactoryCache().getOrAddFactory(
            params.descriptorSets[i].factoryType, std::move(params.descriptorSets[i].factory)));
        descriptorLayouts[i] = descriptorSets[i]->getDescriptorLayout();
    }

    // create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = params.descriptorSets.size();
    pipelineLayoutInfo.pSetLayouts            = descriptorLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = params.pushConstants.size();
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
    descriptors.resize(descriptorSets.size());
    initDescriptorSets(cache, descriptors.data());
}

std::uint32_t PipelineLayout::initDescriptorSets(ds::DescriptorSetInstanceCache& cache,
                                                 ds::DescriptorSetInstance** sets) const {
    for (unsigned int i = 0; i < descriptorSets.size(); ++i) {
        sets[i] = cache.getDescriptorSet(descriptorSets[i]);
    }
    return descriptorSets.size();
}

} // namespace vk
} // namespace rc
} // namespace bl

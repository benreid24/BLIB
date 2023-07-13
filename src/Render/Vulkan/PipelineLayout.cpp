#include <BLIB/Render/Vulkan/PipelineLayout.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
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

std::uint32_t PipelineLayout::updateDescriptorSets(ds::DescriptorSetInstanceCache& cache,
                                                   ds::DescriptorSetInstance** sets,
                                                   std::uint32_t descriptorCount,
                                                   ecs::Entity entity, std::uint32_t sceneId,
                                                   UpdateSpeed updateSpeed) const {
    const scene::Key key{updateSpeed, sceneId};

    // capture old sets
    std::array<ds::DescriptorSetInstance*, 4> ogSets{nullptr, nullptr, nullptr, nullptr};
    for (std::uint32_t i = 0; i < descriptorCount; ++i) { ogSets[i] = sets[i]; }

    // create new sets
    const std::uint32_t newSetCount = initDescriptorSets(cache, sets);

    // call allocate for new descriptor sets
    for (std::uint8_t i = 0; i < newSetCount; ++i) {
        for (std::uint8_t j = 0; j < descriptorCount; ++j) {
            if (ogSets[j] == sets[i]) {
                ogSets[j] = nullptr;
                goto noAdd;
            }
        }

        if (!sets[i]->allocateObject(entity, key)) {
            BL_LOG_ERROR << "Unable to update entity " << entity << " (scene id: " << sceneId
                         << ") to new layout due to descriptor data missing";
        }

    noAdd:
        continue;
    }

    // call remove for ones we no longer use
    for (std::uint8_t i = 0; i < descriptorCount; ++i) {
        if (ogSets[i] != nullptr) { ogSets[i]->releaseObject(entity, key); }
    }

    return newSetCount;
}

} // namespace vk
} // namespace rc
} // namespace bl

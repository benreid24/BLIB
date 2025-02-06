#include <BLIB/Render/Resources/GlobalDescriptors.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>

namespace bl
{
namespace rc
{
namespace res
{
GlobalDescriptors::GlobalDescriptors(Renderer& renderer, TexturePool& texturePool,
                                     MaterialPool& materialPool)
: renderer(renderer)
, texturePool(texturePool)
, materialPool(materialPool) {}

void GlobalDescriptors::bindDescriptors(VkCommandBuffer cb, VkPipelineLayout pipelineLayout,
                                        std::uint32_t setIndex, bool forRt) {
    const VkDescriptorSet ds = forRt ? rtDescriptorSets.current() : descriptorSets.current();
    vkCmdBindDescriptorSets(
        cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, setIndex, 1, &ds, 0, nullptr);
}

void GlobalDescriptors::init() {
    auto& vulkanState = renderer.vulkanState();

    // create descriptor layout
    VkDescriptorSetLayoutBinding setBindings[] = {texturePool.getLayoutBinding(),
                                                  materialPool.getLayoutBinding()};
    constexpr std::size_t NBindings            = std::size(setBindings);
    VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
    descriptorCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorCreateInfo.bindingCount = NBindings;
    descriptorCreateInfo.pBindings    = setBindings;
    if (VK_SUCCESS !=
        vkCreateDescriptorSetLayout(
            vulkanState.device, &descriptorCreateInfo, nullptr, &descriptorSetLayout)) {
        throw std::runtime_error("Failed to create texture pool descriptor set layout");
    }

    // create descriptor pool
    VkDescriptorPoolSize poolSizes[NBindings]{}; // doesn't handle dupes of types
    for (std::size_t i = 0; i < NBindings; ++i) {
        poolSizes[i].type            = setBindings[i].descriptorType;
        poolSizes[i].descriptorCount = setBindings[i].descriptorCount * Config::MaxConcurrentFrames;
    }

    VkDescriptorPoolCreateInfo poolCreate{};
    poolCreate.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreate.maxSets       = 2 * Config::MaxConcurrentFrames;
    poolCreate.poolSizeCount = NBindings;
    poolCreate.pPoolSizes    = poolSizes;
    if (VK_SUCCESS !=
        vkCreateDescriptorPool(vulkanState.device, &poolCreate, nullptr, &descriptorPool)) {
        throw std::runtime_error("Failed to create texture descriptor pool");
    }

    // allocate descriptor set
    VkDescriptorSet allocatedSets[2 * Config::MaxConcurrentFrames];
    std::array<VkDescriptorSetLayout, 2 * Config::MaxConcurrentFrames> setLayouts;
    setLayouts.fill(descriptorSetLayout);

    VkDescriptorSetAllocateInfo setAlloc{};
    setAlloc.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAlloc.descriptorPool     = descriptorPool;
    setAlloc.descriptorSetCount = setLayouts.size();
    setAlloc.pSetLayouts        = setLayouts.data();
    if (VK_SUCCESS != vkAllocateDescriptorSets(vulkanState.device, &setAlloc, allocatedSets)) {
        throw std::runtime_error("Failed to allocate texture descriptor set");
    }

    unsigned int i = 0;
    descriptorSets.init(vulkanState, [&i, &allocatedSets](auto& set) { set = allocatedSets[i++]; });
    rtDescriptorSets.init(vulkanState,
                          [&i, &allocatedSets](auto& set) { set = allocatedSets[i++]; });

    texturePool.init(descriptorSets, rtDescriptorSets);
    materialPool.init(descriptorSets, rtDescriptorSets);
}

void GlobalDescriptors::cleanup() {
    texturePool.cleanup();
    materialPool.cleanup();

    vkDestroyDescriptorPool(renderer.vulkanState().device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(renderer.vulkanState().device, descriptorSetLayout, nullptr);
}

void GlobalDescriptors::onFrameStart() {
    texturePool.onFrameStart(
        descriptorWriter, descriptorSets.current(), rtDescriptorSets.current());
    materialPool.onFrameStart();
    descriptorWriter.performWrite(renderer.vulkanState().device);
}

} // namespace res
} // namespace rc
} // namespace bl
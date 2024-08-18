#include <BLIB/Render/Vulkan/DescriptorSet.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

#include <BLIB/Util/Random.hpp>
#include <unordered_map>

namespace bl
{
namespace rc
{
namespace vk
{
DescriptorSet::DescriptorSet()
: vulkanState(nullptr)
, set(nullptr) {}

DescriptorSet::DescriptorSet(VulkanState& vs)
: vulkanState(&vs)
, set(nullptr) {}

DescriptorSet::DescriptorSet(VulkanState& vs, VkDescriptorSetLayout layout, bool ded)
: DescriptorSet(vs) {
    allocate(layout, ded);
}

DescriptorSet::DescriptorSet(DescriptorSet&& ds)
: vulkanState(ds.vulkanState)
, alloc(ds.alloc)
, set(ds.set) {
    ds.set = nullptr;
}

DescriptorSet::~DescriptorSet() { deferRelease(); }

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& ds) {
    alloc  = ds.alloc;
    set    = ds.set;
    ds.set = nullptr;
    return *this;
}

void DescriptorSet::init(VulkanState& vs) { vulkanState = &vs; }

void DescriptorSet::allocate(VkDescriptorSetLayout layout, bool dedicated) {
    deferRelease();
    alloc = vulkanState->descriptorPool.allocate(layout, &set, 1, dedicated);
}

void DescriptorSet::release() {
    if (set) {
        vulkanState->descriptorPool.release(alloc);
        set = nullptr;
    }
}

void DescriptorSet::deferRelease() {
    if (set) {
        vulkanState->cleanupManager.add([vs = vulkanState, alloc = alloc, set = set]() {
            vs->descriptorPool.release(alloc, &set);
        });
        set = nullptr;
    }
}

void DescriptorSet::bind(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint,
                         VkPipelineLayout layout, std::uint32_t index) {
    vkCmdBindDescriptorSets(commandBuffer, bindPoint, layout, index, 1, &set, 0, nullptr);
}

} // namespace vk
} // namespace rc
} // namespace bl

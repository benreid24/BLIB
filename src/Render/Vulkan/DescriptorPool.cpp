#include <BLIB/Render/Vulkan/DescriptorPool.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <stdexcept>

namespace bl
{
namespace gfx
{
namespace vk
{
namespace
{
constexpr std::array<unsigned int, DescriptorPool::BindingTypeCount> PoolSizes = {
    20,  // VK_DESCRIPTOR_TYPE_SAMPLER
    200, // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    20,  // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
    20,  // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
    20,  // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
    20,  // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
    500, // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    50,  // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    50,  // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    50,  // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
    50   // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
};
constexpr unsigned int MaxSets = 500;
} // namespace

DescriptorPool::SetBindingInfo::SetBindingInfo()
: bindingCount(0) {}

DescriptorPool::DescriptorPool(VulkanState& vs)
: vulkanState(vs) {}

void DescriptorPool::init() { pools.emplace_back(vulkanState); }

void DescriptorPool::cleanup() {
    pools.clear();
    for (auto& pair : layoutMap) {
        vkDestroyDescriptorSetLayout(vulkanState.device, pair.first, nullptr);
    }
}

VkDescriptorSetLayout DescriptorPool::createLayout(const SetBindingInfo& allocInfo) {
    VkDescriptorSetLayout layout;

    VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
    descriptorCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorCreateInfo.bindingCount = allocInfo.bindingCount;
    descriptorCreateInfo.pBindings    = allocInfo.bindings.data();
    if (VK_SUCCESS !=
        vkCreateDescriptorSetLayout(vulkanState.device, &descriptorCreateInfo, nullptr, &layout)) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
    layoutMap.try_emplace(layout, allocInfo);

    return layout;
}

DescriptorPool::AllocationHandle DescriptorPool::allocate(VkDescriptorSetLayout layout,
                                                          VkDescriptorSet* sets,
                                                          std::size_t setCount, bool dedicated) {
    // find layout info
    const auto lit = layoutMap.find(layout);
    if (lit == layoutMap.end()) {
        throw std::runtime_error(
            "Failed to find descriptor set layout. Create the layout with the pool");
    }
    const SetBindingInfo& allocInfo = lit->second;

    // test if requires dedicated pool
    if (!dedicated) {
        // TODO - check sets and binding counts
    }

    // create allocation record
    AllocationHandle handle =
        allocations.emplace(allocations.begin(), dedicated, layout, setCount, sets);

    // create dedicated pool if required
    if (dedicated) {
        handle->pool = pools.emplace(pools.end(), vulkanState, allocInfo, setCount);
        handle->pool->allocate(allocInfo, layout, sets, setCount);
        return handle;
    }

    // see if existing pool can allocate
    for (auto it = pools.begin(); it != pools.end(); ++it) {
        if (it->canAllocate(allocInfo, setCount)) {
            handle->pool = it;
            goto newPoolNotNeeded;
        }
    }

    // create new pool to allocate from
    handle->pool = pools.emplace(pools.end(), vulkanState);

newPoolNotNeeded:
    handle->pool->allocate(allocInfo, layout, sets, setCount);
    return handle;
}

void DescriptorPool::release(AllocationHandle handle, VkDescriptorSet* sets) {
    // find layout info
    const auto lit = layoutMap.find(handle->layout);
    if (lit == layoutMap.end()) {
        throw std::runtime_error(
            "Failed to find descriptor set layout. Create the layout with the pool");
    }
    const SetBindingInfo& allocInfo = lit->second;

    // free sets
    if (handle->dedicated) { pools.erase(handle->pool); }
    else {
        handle->pool->release(allocInfo, sets != nullptr ? sets : handle->sets, handle->setCount);
    }
    allocations.erase(handle);
}

DescriptorPool::Subpool::Subpool(VulkanState& vs)
: vulkanState(vs)
, freeSets(MaxSets)
, available(PoolSizes) {
    std::array<VkDescriptorPoolSize, PoolSizes.size()> sizes;
    for (std::size_t i = 0; i < PoolSizes.size(); ++i) {
        sizes[i].type            = static_cast<VkDescriptorType>(i);
        sizes[i].descriptorCount = PoolSizes[i];
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = sizes.size();
    poolInfo.pPoolSizes    = sizes.data();
    poolInfo.maxSets       = MaxSets;
    poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    if (vkCreateDescriptorPool(vs.device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

DescriptorPool::Subpool::Subpool(VulkanState& vs, const SetBindingInfo& allocInfo,
                                 std::size_t setCount)
: vulkanState(vs)
, freeSets(0) {
    auto bindings = allocInfo.bindings;
    for (auto& binding : bindings) { binding.descriptorCount *= setCount; }

    std::array<VkDescriptorPoolSize, Config::MaxDescriptorSets> poolSizes;
    for (unsigned int i = 0; i < allocInfo.bindingCount; ++i) {
        poolSizes[i].type            = allocInfo.bindings[i].descriptorType;
        poolSizes[i].descriptorCount = allocInfo.bindings[i].descriptorCount;
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = allocInfo.bindingCount;
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = setCount;
    if (vkCreateDescriptorPool(vs.device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

DescriptorPool::Subpool::~Subpool() { vkDestroyDescriptorPool(vulkanState.device, pool, nullptr); }

bool DescriptorPool::Subpool::canAllocate(const SetBindingInfo& allocInfo,
                                          std::size_t setCount) const {
    if (freeSets < setCount) { return false; }

    for (std::size_t i = 0; i < setCount; ++i) {
        for (std::size_t j = 0; j < allocInfo.bindingCount; ++j) {
            const auto& binding = allocInfo.bindings[j];
            if (available[binding.descriptorType] < binding.descriptorCount) { return false; }
        }
    }

    return true;
}

void DescriptorPool::Subpool::allocate(const SetBindingInfo& allocInfo,
                                       VkDescriptorSetLayout layout, VkDescriptorSet* sets,
                                       std::size_t setCount) {
    // update metadata
    freeSets -= setCount;
    for (std::size_t j = 0; j < allocInfo.bindingCount; ++j) {
        const auto& binding = allocInfo.bindings[j];
        available[binding.descriptorType] -= binding.descriptorCount * setCount;
    }

    // duplicate layout for vulkan api
    std::array<VkDescriptorSetLayout, 128> layoutsArray;
    std::vector<VkDescriptorSetLayout> layoutsVector;
    VkDescriptorSetLayout* pLayouts;
    if (setCount <= layoutsArray.size()) {
        for (unsigned int i = 0; i < setCount; ++i) { layoutsArray[i] = layout; }
        pLayouts = layoutsArray.data();
    }
    else {
        layoutsVector.resize(setCount, layout);
        pLayouts = layoutsVector.data();
    }

    // allocate sets
    VkDescriptorSetAllocateInfo setAllocInfo{};
    setAllocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.descriptorPool     = pool;
    setAllocInfo.descriptorSetCount = setCount;
    setAllocInfo.pSetLayouts        = pLayouts;
    if (vkAllocateDescriptorSets(vulkanState.device, &setAllocInfo, sets) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets");
    }
}

void DescriptorPool::Subpool::release(const SetBindingInfo& allocInfo, VkDescriptorSet* sets,
                                      std::size_t setCount) {
    // update metadata
    freeSets += setCount;
    for (std::size_t j = 0; j < allocInfo.bindingCount; ++j) {
        const auto& binding = allocInfo.bindings[j];
        available[binding.descriptorType] += binding.descriptorCount * setCount;
    }

    // free sets
    vkCheck(vkFreeDescriptorSets(vulkanState.device, pool, setCount, sets));
}

} // namespace vk
} // namespace gfx
} // namespace bl

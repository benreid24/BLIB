#include <BLIB/Render/Vulkan/DescriptorPool.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <stdexcept>

namespace bl
{
namespace render
{
namespace
{
constexpr std::array<unsigned int, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1> PoolSizes = {
    20,  // VK_DESCRIPTOR_TYPE_SAMPLER
    500, // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    20,  // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
    20,  // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
    20,  // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
    20,  // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
    200, // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    50,  // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    50,  // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    50,  // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
    50   // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
};
constexpr unsigned int MaxSets = 500;
} // namespace

DescriptorPool::DescriptorPool(VulkanState& vs)
: vulkanState(vs) {}

void DescriptorPool::init() {
    pools.emplace_back(vulkanState, true);
    foreverPools.emplace_back(vulkanState, false);
}

void DescriptorPool::cleanup() {
    pools.clear();
    foreverPools.clear();
}

void DescriptorPool::allocateForever(const VkDescriptorSetLayoutCreateInfo** createInfos,
                                     const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                                     std::size_t setCount) {
    doAllocate(foreverPools, createInfos, layouts, sets, setCount);
}

DescriptorPool::AllocationHandle DescriptorPool::allocate(
    const VkDescriptorSetLayoutCreateInfo** createInfos, const VkDescriptorSetLayout* layouts,
    VkDescriptorSet* sets, std::size_t setCount) {
    return doAllocate(pools, createInfos, layouts, sets, setCount);
}

void DescriptorPool::release(AllocationHandle handle,
                             const VkDescriptorSetLayoutCreateInfo** createInfos,
                             const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                             std::size_t setCount) {
    handle->release(createInfos, layouts, sets, setCount);
    if (pools.size() > 1 && !handle->inUse()) { pools.erase(std::next(handle).base()); }
}

DescriptorPool::AllocationHandle DescriptorPool::doAllocate(
    std::list<Subpool>& poolList, const VkDescriptorSetLayoutCreateInfo** createInfos,
    const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets, std::size_t setCount) {
// test that allocation is possible
#ifdef BLIB_DEBUG
    if (setCount > MaxSets) {
        BL_LOG_CRITICAL << "Attemptiing to allocate " << setCount
                        << " descriptor sets but pools can only allocate " << MaxSets;
        throw std::runtime_error("Trying to allocate too many descriptor sets");
    }

    for (unsigned int i = 0; i < setCount; ++i) {
        for (unsigned int j = 0; j < createInfos[i]->bindingCount; ++j) {
            const auto& binding = createInfos[i]->pBindings[j];
            if (binding.descriptorType >= PoolSizes.size()) {
                BL_LOG_CRITICAL << "Trying to allocate unsupported descriptor type: "
                                << binding.descriptorType;
                throw std::runtime_error("Trying to allocate unsupported descriptor type");
            }
            if (binding.descriptorCount > PoolSizes[binding.descriptorType]) {
                BL_LOG_CRITICAL << "Trying to allocate " << binding.descriptorCount
                                << " descriptors of type " << binding.descriptorType
                                << " but the max is " << PoolSizes[binding.descriptorType];
                throw std::runtime_error("Trying to allocate too many descriptors");
            }
        }
    }
#endif

    // see if existing pool can allocate
    for (auto it = poolList.rbegin(); it != poolList.rend(); ++it) {
        if (it->canAllocate(createInfos, setCount)) {
            it->allocate(createInfos, layouts, sets, setCount);
            return it;
        }
    }

    // create new pool to allocate from
    poolList.emplace_back(vulkanState, &poolList == &pools);
    poolList.back().allocate(createInfos, layouts, sets, setCount);
    return poolList.rbegin();
}

DescriptorPool::Subpool::Subpool(VulkanState& vs, bool allowFree)
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
    if (allowFree) { poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; }
    if (vkCreateDescriptorPool(vs.device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

DescriptorPool::Subpool::~Subpool() { vkDestroyDescriptorPool(vulkanState.device, pool, nullptr); }

bool DescriptorPool::Subpool::canAllocate(const VkDescriptorSetLayoutCreateInfo** createInfos,
                                          std::size_t setCount) const {
    if (freeSets < setCount) { return false; }

    for (std::size_t i = 0; i < setCount; ++i) {
        for (std::size_t j = 0; j < createInfos[i]->bindingCount; ++j) {
            const auto& binding = createInfos[i]->pBindings[j];
            if (available[binding.descriptorType] < binding.descriptorCount) { return false; }
        }
    }

    return true;
}

void DescriptorPool::Subpool::allocate(const VkDescriptorSetLayoutCreateInfo** createInfos,
                                       const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                                       std::size_t setCount) {
    // update metadata
    freeSets -= setCount;
    for (std::size_t i = 0; i < setCount; ++i) {
        for (std::size_t j = 0; j < createInfos[i]->bindingCount; ++j) {
            const auto& binding = createInfos[i]->pBindings[j];
            available[binding.descriptorType] -= binding.descriptorCount;
        }
    }

    // allocate sets
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = pool;
    allocInfo.descriptorSetCount = setCount;
    allocInfo.pSetLayouts        = layouts;
    if (vkAllocateDescriptorSets(vulkanState.device, &allocInfo, sets) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets");
    }
}

void DescriptorPool::Subpool::release(const VkDescriptorSetLayoutCreateInfo** createInfos,
                                      const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                                      std::size_t setCount) {
    // update metadata
    freeSets += setCount;
    for (std::size_t i = 0; i < setCount; ++i) {
        for (std::size_t j = 0; j < createInfos[i]->bindingCount; ++j) {
            const auto& binding = createInfos[i]->pBindings[j];
            available[binding.descriptorType] += binding.descriptorCount;
        }
    }

    // free sets
    vkFreeDescriptorSets(vulkanState.device, pool, setCount, sets);
}

bool DescriptorPool::Subpool::inUse() const {
    if (freeSets != MaxSets) { return true; }

    for (std::size_t i = 0; i < PoolSizes.size(); ++i) {
        if (available[i] != PoolSizes[i]) { return true; }
    }

    return false;
}

} // namespace render
} // namespace bl

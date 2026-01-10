#include <BLIB/Render/Vulkan/DescriptorPool.hpp>

#include <BLIB/Render/Vulkan/VulkanLayer.hpp>
#include <stdexcept>

namespace bl
{
namespace rc
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
    200, // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    200, // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
    100, // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    50,  // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
    10   // VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
};
constexpr unsigned int MaxSets = 500;

bool canDefaultPoolAllocate(unsigned int setCount,
                            const DescriptorPool::SetBindingInfo& allocInfo) {
    if (setCount > MaxSets) { return false; }

    auto available = PoolSizes;

    for (std::uint32_t i = 0; i < allocInfo.bindingCount; ++i) {
        const auto& binding             = allocInfo.bindings[i];
        const unsigned int bindingCount = binding.descriptorCount * setCount;
        if (bindingCount > available[binding.descriptorType]) { return false; }
        available[binding.descriptorType] -= bindingCount;
    }
    return true;
}
} // namespace

DescriptorPool::SetBindingInfo::SetBindingInfo()
: bindingCount(0)
, bindings{} {
    for (unsigned int i = 0; i < cfg::Limits::MaxDescriptorBindings; ++i) {
        bindings[i].binding = i;
    }
}

DescriptorPool::DescriptorPool(VulkanLayer& vs)
: vulkanState(vs) {}

void DescriptorPool::init() { pools.emplace_back(vulkanState); }

void DescriptorPool::cleanup() {
    pools.clear();
    for (auto& pair : layoutMap) {
        vkDestroyDescriptorSetLayout(vulkanState.getDevice(), pair.first, nullptr);
    }
}

VkDescriptorSetLayout DescriptorPool::createLayout(const SetBindingInfo& allocInfo) {
    std::unique_lock lock(mutex);

    VkDescriptorSetLayout layout;
    VkDescriptorSetLayoutCreateInfo descriptorCreateInfo{};
    descriptorCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorCreateInfo.bindingCount = allocInfo.bindingCount;
    descriptorCreateInfo.pBindings    = allocInfo.bindings.data();
    if (VK_SUCCESS != vkCreateDescriptorSetLayout(
                          vulkanState.getDevice(), &descriptorCreateInfo, nullptr, &layout)) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
    layoutMap.try_emplace(layout, allocInfo);

    return layout;
}

DescriptorPool::AllocationHandle DescriptorPool::allocate(VkDescriptorSetLayout layout,
                                                          VkDescriptorSet* sets,
                                                          std::size_t setCount, bool dedicated) {
    std::unique_lock lock(mutex);

    // find layout info
    const auto lit = layoutMap.find(layout);
    if (lit == layoutMap.end()) {
        throw std::runtime_error(
            "Failed to find descriptor set layout. Create the layout with the pool");
    }
    const SetBindingInfo& allocInfo = lit->second;

    // test if requires dedicated pool
    if (!dedicated) {
        if (!canDefaultPoolAllocate(setCount, allocInfo)) {
            BL_LOG_WARN << "Descriptor set allocation requires dedicated pool due to size";
            dedicated = true;
        }
    }

    // create allocation record
    AllocationHandle handle = AllocationHandle(
        this, allocations.emplace(allocations.begin(), dedicated, layout, setCount, sets));

    // create dedicated pool if required
    if (dedicated) {
        handle.it->pool = pools.emplace(pools.end(), vulkanState, allocInfo, setCount);
        handle.it->pool->allocate(allocInfo, layout, sets, setCount);
        return handle;
    }

    // see if existing pool can allocate
    for (auto it = pools.begin(); it != pools.end(); ++it) {
        if (it->canAllocate(allocInfo, setCount)) {
            handle.it->pool = it;
            goto newPoolNotNeeded;
        }
    }

    // create new pool to allocate from
    handle.it->pool = pools.emplace(pools.end(), vulkanState);

newPoolNotNeeded:
    handle.it->pool->allocate(allocInfo, layout, sets, setCount);
    return handle;
}

void DescriptorPool::release(AllocationHandle& handle, const VkDescriptorSet* sets) {
    if (!handle.pool) { return; }
    if (handle.pool != this) {
        BL_LOG_ERROR << "Attempted to release descriptor set from wrong pool";
        return;
    }

    std::unique_lock lock(mutex);

    // find layout info
    const auto lit = layoutMap.find(handle.it->layout);
    if (lit == layoutMap.end()) {
        throw std::runtime_error(
            "Failed to find descriptor set layout. Create the layout with the pool");
    }
    const SetBindingInfo& allocInfo = lit->second;

    // free sets
    if (handle.it->dedicated) { pools.erase(handle.it->pool); }
    else {
        handle.it->pool->release(
            allocInfo, sets != nullptr ? sets : handle.it->sets, handle.it->setCount);
    }
    allocations.erase(handle.it);

    handle.pool = nullptr;
}

DescriptorPool::Subpool::Subpool(VulkanLayer& vs)
: vulkanState(vs)
, freeSets(MaxSets)
, available(PoolSizes) {
    BL_LOG_INFO << "Allocated shared pool: " << this;

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
    if (vkCreateDescriptorPool(vs.getDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

DescriptorPool::Subpool::Subpool(VulkanLayer& vs, const SetBindingInfo& allocInfo,
                                 std::size_t setCount)
: vulkanState(vs)
, freeSets(0) {
    BL_LOG_INFO << "Allocated dedicated pool: " << this;

    auto bindings = allocInfo.bindings;
    for (auto& binding : bindings) { binding.descriptorCount *= setCount; }

    std::array<VkDescriptorPoolSize, cfg::Limits::MaxDescriptorBindings> poolSizes;
    for (unsigned int i = 0; i < allocInfo.bindingCount; ++i) {
        poolSizes[i].type            = allocInfo.bindings[i].descriptorType;
        poolSizes[i].descriptorCount = allocInfo.bindings[i].descriptorCount * setCount;
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = allocInfo.bindingCount;
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = setCount;
    if (vkCreateDescriptorPool(vs.getDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

DescriptorPool::Subpool::~Subpool() {
    BL_LOG_INFO << "Released pool: " << this;
    vkDestroyDescriptorPool(vulkanState.getDevice(), pool, nullptr);
}

bool DescriptorPool::Subpool::canAllocate(const SetBindingInfo& allocInfo,
                                          std::size_t setCount) const {
    if (freeSets < setCount) { return false; }

    auto availLocal = available;

    for (std::size_t j = 0; j < allocInfo.bindingCount; ++j) {
        const auto& binding             = allocInfo.bindings[j];
        const unsigned int bindingCount = binding.descriptorCount * setCount;
        if (availLocal[binding.descriptorType] < bindingCount) { return false; }
        availLocal[binding.descriptorType] -= bindingCount;
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
    if (vkAllocateDescriptorSets(vulkanState.getDevice(), &setAllocInfo, sets) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets");
    }
}

void DescriptorPool::Subpool::release(const SetBindingInfo& allocInfo, const VkDescriptorSet* sets,
                                      std::size_t setCount) {
    // update metadata
    freeSets += setCount;
    for (std::size_t j = 0; j < allocInfo.bindingCount; ++j) {
        const auto& binding = allocInfo.bindings[j];
        available[binding.descriptorType] += binding.descriptorCount * setCount;
    }

    // free sets
    vkCheck(vkFreeDescriptorSets(vulkanState.getDevice(), pool, setCount, sets));
}

DescriptorPool::AllocationHandle::AllocationHandle()
: pool(nullptr) {}

DescriptorPool::AllocationHandle::AllocationHandle(AllocationHandle&& other)
: pool(other.pool)
, it(other.it) {
    other.pool = nullptr;
}

DescriptorPool::AllocationHandle::AllocationHandle(const AllocationHandle& other)
: pool(other.pool)
, it(other.it) {}

DescriptorPool::AllocationHandle::AllocationHandle(DescriptorPool* p,
                                                   std::list<Allocation>::iterator allocIt)
: pool(p)
, it(allocIt) {}

DescriptorPool::AllocationHandle& DescriptorPool::AllocationHandle::operator=(
    AllocationHandle&& other) {
    if (this != &other) {
        pool       = other.pool;
        it         = other.it;
        other.pool = nullptr;
    }
    return *this;
}

DescriptorPool::AllocationHandle& DescriptorPool::AllocationHandle::operator=(
    const AllocationHandle& other) {
    if (this != &other) {
        pool = other.pool;
        it   = other.it;
    }
    return *this;
}

DescriptorPool::AllocationHandle::~AllocationHandle() {
    if (pool) { release(); }
}

void DescriptorPool::AllocationHandle::release(const VkDescriptorSet* sets) {
    if (pool) {
        pool->release(*this, sets);
        pool = nullptr;
    }
}

} // namespace vk
} // namespace rc
} // namespace bl

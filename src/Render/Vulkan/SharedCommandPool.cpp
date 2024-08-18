#include <BLIB/Render/Vulkan/SharedCommandPool.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
SharedCommandPool::SharedCommandPool()
: vs(nullptr) {
    allocations.reserve(4);
}

void SharedCommandPool::create(VulkanState& vulkanState) {
    vs = &vulkanState;
    pool.init(vulkanState, [&vulkanState](VkCommandPool& pool) {
        pool = vulkanState.createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    });
}

void SharedCommandPool::onFrameStart() { vkResetCommandPool(vs->device, pool.current(), 0); }

void SharedCommandPool::cleanup() {
    for (auto& alloc : allocations) { vkDestroyFence(vs->device, alloc.fence, nullptr); }
    pool.cleanup([this](VkCommandPool p) { vkDestroyCommandPool(vs->device, p, nullptr); });
}

SharedCommandBuffer SharedCommandPool::createBuffer(VkCommandBufferUsageFlags flags) {
    mutex.lock();

    for (auto it = allocations.begin(); it != allocations.end();) {
        if (VK_SUCCESS == vkGetFenceStatus(vs->device, it->fence)) {
            vkFreeCommandBuffers(vs->device, it->pool, 1, &it->buffer);
            vkDestroyFence(vs->device, it->fence, nullptr);
            it = allocations.erase(it);
        }
        else { ++it; }
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = pool.current();
    allocInfo.commandBufferCount = 1;

    auto& alloc                    = allocations.emplace_back();
    VkCommandBuffer& commandBuffer = alloc.buffer;
    VkFence& fence                 = alloc.fence;
    alloc.pool                     = pool.current();

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkCheck(vkCreateFence(vs->device, &fenceInfo, nullptr, &fence));
    vkCheck(vkAllocateCommandBuffers(vs->device, &allocInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags | VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkCheck(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    return {this, fence, commandBuffer};
}

void SharedCommandPool::submit(SharedCommandBuffer& buffer) {
    VkCommandBuffer lb = buffer;
    vkCheck(vkEndCommandBuffer(buffer));

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &lb;

    vs->submitCommandBuffer(submitInfo, buffer.fence);

    mutex.unlock();
}

} // namespace vk
} // namespace rc
} // namespace bl

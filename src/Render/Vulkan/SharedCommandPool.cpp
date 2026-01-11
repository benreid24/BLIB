#include <BLIB/Render/Vulkan/SharedCommandPool.hpp>

#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

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

void SharedCommandPool::create(VulkanLayer& vulkanState) {
    vs = &vulkanState;
    pool.init(vulkanState, [&vulkanState](VkCommandPool& pool) {
        pool = vulkanState.createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    });
}

void SharedCommandPool::onFrameStart() { vkResetCommandPool(vs->getDevice(), pool.current(), 0); }

void SharedCommandPool::cleanup() {
    for (auto& alloc : allocations) { vkDestroyFence(vs->getDevice(), alloc.fence, nullptr); }
    pool.cleanup([this](VkCommandPool p) { vkDestroyCommandPool(vs->getDevice(), p, nullptr); });
}

SharedCommandBuffer SharedCommandPool::createBuffer(VkCommandBufferUsageFlags flags) {
    std::unique_lock lock(mutex);

    for (auto it = allocations.begin(); it != allocations.end();) {
        if (VK_SUCCESS == vkGetFenceStatus(vs->getDevice(), it->fence)) {
            vkFreeCommandBuffers(vs->getDevice(), it->pool, 1, &it->buffer);
            vkDestroyFence(vs->getDevice(), it->fence, nullptr);
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
    vkCheck(vkCreateFence(vs->getDevice(), &fenceInfo, nullptr, &fence));
    vkCheck(vkAllocateCommandBuffers(vs->getDevice(), &allocInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags | VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkCheck(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    return {this, std::move(lock), fence, commandBuffer};
}

void SharedCommandPool::submit(SharedCommandBuffer& buffer) {
    VkCommandBuffer lb = buffer;
    vkCheck(vkEndCommandBuffer(buffer));

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &lb;

    vs->submitCommandBuffer(submitInfo, buffer.fence);
    buffer.lock.unlock();
}

} // namespace vk
} // namespace rc
} // namespace bl

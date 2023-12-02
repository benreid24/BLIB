#include <BLIB/Render/Vulkan/SharedCommandPool.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
SharedCommandPool::SharedCommandPool()
: vs(nullptr) {}

void SharedCommandPool::create(VulkanState& vulkanState) {
    vs = &vulkanState;
    pool.init(vulkanState, [&vulkanState](VkCommandPool& pool) {
        pool = vulkanState.createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    });
}

void SharedCommandPool::onFrameStart() { vkResetCommandPool(vs->device, pool.current(), 0); }

void SharedCommandPool::cleanup() {
    pool.cleanup([this](VkCommandPool p) { vkDestroyCommandPool(vs->device, p, nullptr); });
}

SharedCommandBuffer SharedCommandPool::createBuffer(VkCommandBufferUsageFlags flags) {
    mutex.lock();

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = pool.current();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkCheck(vkAllocateCommandBuffers(vs->device, &allocInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags | VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkCheck(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    return {this, commandBuffer};
}

void SharedCommandPool::submit(SharedCommandBuffer& buffer) {
    VkCommandBuffer lb = buffer;
    vkCheck(vkEndCommandBuffer(buffer));

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &lb;

    vs->submitCommandBuffer(submitInfo);

    mutex.unlock();
}

} // namespace vk
} // namespace rc
} // namespace bl

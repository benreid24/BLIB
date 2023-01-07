#include <BLIB/Render/Vulkan/SwapFrame.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
SwapFrame::~SwapFrame() {
    if (deviceInitedWith) { cleanup(); }
}

void SwapFrame::initialize(VulkanState& vs) {
    deviceInitedWith = vs.device;

    commandPool = vs.createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(deviceInitedWith, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(deviceInitedWith, &semaphoreInfo, nullptr, &imageAvailableSemaphore) !=
            VK_SUCCESS ||
        vkCreateSemaphore(deviceInitedWith, &semaphoreInfo, nullptr, &renderFinishedSemaphore) !=
            VK_SUCCESS ||
        vkCreateFence(deviceInitedWith, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphores");
    }
}

void SwapFrame::cleanup() {
    vkDestroySemaphore(deviceInitedWith, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(deviceInitedWith, imageAvailableSemaphore, nullptr);
    vkDestroyFence(deviceInitedWith, inFlightFence, nullptr);
    deviceInitedWith = nullptr;
}

} // namespace render
} // namespace bl

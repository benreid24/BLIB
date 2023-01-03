#include <BLIB/Render/Vulkan/SwapFrame.hpp>

namespace bl
{
namespace render
{
SwapFrame::SwapFrame(VkDevice device, VkCommandPool commandPool) {
    initialize(device, commandPool);
}

SwapFrame::~SwapFrame() {
    if (deviceInitedWith) { cleanup(); }
}

void SwapFrame::initialize(VkDevice device, VkCommandPool commandPool) {
    deviceInitedWith = device;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) !=
            VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) !=
            VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
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

#include <BLIB/Render/Vulkan/TransferEngine.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
TransferEngine::TransferEngine(VulkanState& vs)
: vulkanState(vs)
, commandBuffer(nullptr) {
    queuedItems.reserve(32);
    stagingBuffers.reserve(32);
    stagingMemory.reserve(32);
    memoryBarriers.reserve(32);
    bufferBarriers.reserve(32);
    imageBarriers.reserve(32);
}

void TransferEngine::init() {
    VkFenceCreateInfo create{};
    create.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(vulkanState.device, &create, nullptr, &fence);
}

void TransferEngine::cleanup() { vkDestroyFence(vulkanState.device, fence, nullptr); }

void TransferEngine::executeTransfers() {
    std::unique_lock lock(mutex);

    // wait for prior transfers to complete before freeing resources
    vkWaitForFences(vulkanState.device, 1, &fence, VK_TRUE, 0);
    vkResetFences(vulkanState.device, 1, &fence);
    releaseStagingBuffers();
    if (commandBuffer) {
        vkFreeCommandBuffers(vulkanState.device, vulkanState.sharedCommandPool, 1, &commandBuffer);
        commandBuffer = nullptr;
    }

    if (!queuedItems.empty()) {
        // create one time command buffer
        VkCommandBufferAllocateInfo alloc{};
        alloc.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc.commandPool        = vulkanState.sharedCommandPool;
        alloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc.commandBufferCount = 1;
        vkAllocateCommandBuffers(vulkanState.device, &alloc, &commandBuffer);

        VkCommandBufferBeginInfo begin{};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &begin);

        // queue transfer commands
        for (Transferable* item : queuedItems) { item->executeTransfer(commandBuffer, *this); }
        queuedItems.clear();

        // one unified sync
        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             0,
                             memoryBarriers.size(),
                             memoryBarriers.data(),
                             bufferBarriers.size(),
                             bufferBarriers.data(),
                             imageBarriers.size(),
                             imageBarriers.data());

        // submit transfer commands
        vkEndCommandBuffer(commandBuffer);
        VkSubmitInfo submit{};
        submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pCommandBuffers    = &commandBuffer;
        submit.commandBufferCount = 1;
        vkQueueSubmit(vulkanState.graphicsQueue, 1, &submit, fence);

        // cleanup
        memoryBarriers.clear();
        bufferBarriers.clear();
        imageBarriers.clear();
    }
};

void TransferEngine::queueTransfer(Transferable& item) {
    std::unique_lock lock(mutex);
    queuedItems.emplace_back(&item);
}

void TransferEngine::createStagingBuffer(VkDeviceSize size, VkBuffer& bufferHandle,
                                         VkDeviceMemory& memoryHandle) {
    vulkanState.createBuffer(size,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             bufferHandle,
                             memoryHandle);
    stagingBuffers.emplace_back(bufferHandle);
    stagingMemory.emplace_back(memoryHandle);
}

void TransferEngine::releaseStagingBuffers() {
    for (VkBuffer buffer : stagingBuffers) { vkDestroyBuffer(vulkanState.device, buffer, nullptr); }
    for (VkDeviceMemory memory : stagingMemory) {
        vkFreeMemory(vulkanState.device, memory, nullptr);
    }
    stagingBuffers.clear();
    stagingMemory.clear();
}

void TransferEngine::registerMemoryBarrier(const VkMemoryBarrier& barrier) {
    memoryBarriers.emplace_back(barrier);
}

void TransferEngine::registerBufferBarrier(const VkBufferMemoryBarrier& barrier) {
    bufferBarriers.emplace_back(barrier);
}

void TransferEngine::registerImageBarrier(const VkImageMemoryBarrier& barrier) {
    imageBarriers.emplace_back(barrier);
}

} // namespace render
} // namespace bl

#include <BLIB/Render/Transfers/TransferEngine.hpp>

#include <BLIB/Render/Transfers/TransferContext.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
namespace tfr
{
TransferEngine::Bucket::Bucket(VulkanState& vs)
: vulkanState(vs) {
    oneTimeItems.reserve(32);
    everyFrameItems.reserve(32);
    stagingBuffers.reserve(32);
    stagingMemory.reserve(32);
    memoryBarriers.reserve(32);
    bufferBarriers.reserve(32);
    imageBarriers.reserve(32);
}

void TransferEngine::Bucket::init() {
    VkFenceCreateInfo create{};
    create.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(vulkanState.device, &create, nullptr, &fence);

    VkCommandBufferAllocateInfo alloc{};
    alloc.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc.commandPool        = vulkanState.sharedCommandPool;
    alloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc.commandBufferCount = 1;
    vkAllocateCommandBuffers(vulkanState.device, &alloc, &commandBuffer);
}

void TransferEngine::Bucket::cleanup() {
    vkDestroyFence(vulkanState.device, fence, nullptr);
    vkFreeCommandBuffers(vulkanState.device, vulkanState.sharedCommandPool, 1, &commandBuffer);
}

TransferEngine::TransferEngine(VulkanState& vs)
: vulkanState(vs)
, immediateBucket(vs)
, frameBucket(vs) {}

void TransferEngine::init() {
    frameBucket.init();
    immediateBucket.init();
}

void TransferEngine::cleanup() {
    frameBucket.cleanup();
    immediateBucket.cleanup();
}

void TransferEngine::executeTransfers() {
    std::unique_lock lock(mutex);

    if (immediateBucket.hasTransfers()) { immediateBucket.executeTransfers(); }
    else { immediateBucket.resetResourcesWithSync(); }

    if (frameBucket.hasTransfers()) {
        vkDeviceWaitIdle(vulkanState.device); // TODO - would pipeline barriers work instead?
        frameBucket.executeTransfers();
    }
    else { frameBucket.resetResourcesWithSync(); }
}

void TransferEngine::Bucket::executeTransfers() {
    // wait for prior transfers to complete before freeing resources
    resetResourcesWithSync();

    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &begin);

    // queue transfer commands
    TransferContext context(
        vulkanState, stagingBuffers, stagingMemory, memoryBarriers, bufferBarriers, imageBarriers);
    for (Transferable* item : everyFrameItems) { item->executeTransfer(commandBuffer, context); }
    for (Transferable* item : oneTimeItems) { item->executeTransfer(commandBuffer, context); }
    oneTimeItems.clear();

    // one unified sync
    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, // correct?
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

void TransferEngine::Bucket::resetResourcesWithSync() {
    vkWaitForFences(vulkanState.device, 1, &fence, VK_TRUE, 0);
    vkResetFences(vulkanState.device, 1, &fence);
    vkResetCommandBuffer(commandBuffer, 0);

    for (VkBuffer buffer : stagingBuffers) { vkDestroyBuffer(vulkanState.device, buffer, nullptr); }
    for (VkDeviceMemory memory : stagingMemory) {
        vkFreeMemory(vulkanState.device, memory, nullptr);
    }
    stagingBuffers.clear();
    stagingMemory.clear();
}

void TransferEngine::queueOneTimeTransfer(Transferable* item,
                                          Transferable::SyncRequirement syncReq) {
    std::unique_lock lock(mutex);
    switch (syncReq) {
    case Transferable::SyncRequirement::DeviceIdle:
        frameBucket.oneTimeItems.emplace_back(item);
        break;
    case Transferable::SyncRequirement::Immediate:
        immediateBucket.oneTimeItems.emplace_back(item);
        break;
    }
}

void TransferEngine::registerPerFrameTransfer(Transferable* item,
                                              Transferable::SyncRequirement syncReq) {
    std::unique_lock lock(mutex);
    switch (syncReq) {
    case Transferable::SyncRequirement::DeviceIdle:
        frameBucket.everyFrameItems.emplace_back(item);
        break;
    case Transferable::SyncRequirement::Immediate:
        immediateBucket.everyFrameItems.emplace_back(item);
        break;
    }
}

void TransferEngine::unregisterPerFrameTransfer(Transferable* item,
                                                Transferable::SyncRequirement syncReq) {
    std::unique_lock lock(mutex);
    Bucket& bucket =
        syncReq == Transferable::SyncRequirement::Immediate ? immediateBucket : frameBucket;
    for (int i = 0; i < bucket.everyFrameItems.size(); ++i) {
        if (bucket.everyFrameItems[i] == item) {
            bucket.everyFrameItems.erase(bucket.everyFrameItems.begin() + i);
            --i;
        }
    }
}

} // namespace tfr
} // namespace render
} // namespace bl

#include <BLIB/Render/Transfers/TransferEngine.hpp>

#include <BLIB/Render/Transfers/TransferContext.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace gfx
{
namespace tfr
{
TransferEngine::Bucket::Bucket(vk::VulkanState& vs)
: vulkanState(vs) {
    oneTimeItems.reserve(32);
    everyFrameItems.reserve(32);
    stagingBuffers.init(vs, [](std::vector<VkBuffer>& bufs) { bufs.reserve(32); });
    stagingAllocs.init(vs, [](std::vector<VmaAllocation>& mems) { mems.reserve(32); });
    memoryBarriers.reserve(32);
    bufferBarriers.reserve(32);
    imageBarriers.reserve(32);
}

void TransferEngine::Bucket::init() {
    fence.init(vulkanState, [this](VkFence& f) {
        VkFenceCreateInfo create{};
        create.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        create.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        vkCheck(vkCreateFence(vulkanState.device, &create, nullptr, &f));
    });

    commandBuffer.emptyInit(vulkanState);
    VkCommandBufferAllocateInfo alloc{};
    alloc.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc.commandPool        = vulkanState.sharedCommandPool;
    alloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc.commandBufferCount = Config::MaxConcurrentFrames;
    vkCheck(vkAllocateCommandBuffers(vulkanState.device, &alloc, commandBuffer.rawData()));
}

void TransferEngine::Bucket::cleanup() {
    fence.cleanup([this](VkFence f) { vkDestroyFence(vulkanState.device, f, nullptr); });
    vkFreeCommandBuffers(vulkanState.device,
                         vulkanState.sharedCommandPool,
                         Config::MaxConcurrentFrames,
                         commandBuffer.rawData());
}

TransferEngine::TransferEngine(vk::VulkanState& vs)
: vulkanState(vs)
, immediateBucket(vs)
, frameBucket(vs) {
    tempVertices.reserve(64);
    tempIndices.reserve(64);
}

void TransferEngine::init() {
    frameBucket.init();
    immediateBucket.init();
}

void TransferEngine::cleanup() {
    frameBucket.cleanup();
    immediateBucket.cleanup();
}

prim::Vertex* TransferEngine::createOneTimeVertexStorage(std::uint32_t count) {
    const std::uint32_t start = tempVertices.size();
    tempVertices.resize(tempVertices.size() + count);
    return &tempVertices[start];
}

std::uint32_t* TransferEngine::createOneTimeIndexStorage(std::uint32_t count) {
    const std::uint32_t start = tempIndices.size();
    tempIndices.resize(tempIndices.size() + count);
    return &tempIndices[start];
}

void TransferEngine::executeTransfers() {
    std::unique_lock lock(mutex);

    if (immediateBucket.hasTransfers()) { immediateBucket.executeTransfers(); }
    else { immediateBucket.resetResourcesWithSync(); }

    if (frameBucket.hasTransfers()) {
        vkCheck(vkDeviceWaitIdle(vulkanState.device));
        frameBucket.executeTransfers();
    }
    else { frameBucket.resetResourcesWithSync(); }

    tempVertices.clear();
    tempIndices.clear();
}

void TransferEngine::Bucket::executeTransfers() {
    // wait for prior transfers to complete before freeing resources
    resetResourcesWithSync();

    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkCheck(vkBeginCommandBuffer(commandBuffer.current(), &begin));

    // queue transfer commands
    TransferContext context(vulkanState,
                            stagingBuffers.current(),
                            stagingAllocs.current(),
                            memoryBarriers,
                            bufferBarriers,
                            imageBarriers);
    for (Transferable* item : everyFrameItems) {
        item->executeTransfer(commandBuffer.current(), context);
    }
    for (Transferable* item : oneTimeItems) {
        item->executeTransfer(commandBuffer.current(), context);
    }
    oneTimeItems.clear();

    // one unified sync
    vkCmdPipelineBarrier(commandBuffer.current(),
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                         VK_DEPENDENCY_BY_REGION_BIT,
                         memoryBarriers.size(),
                         memoryBarriers.data(),
                         bufferBarriers.size(),
                         bufferBarriers.data(),
                         imageBarriers.size(),
                         imageBarriers.data());

    // submit transfer commands
    vkCheck(vkEndCommandBuffer(commandBuffer.current()));
    VkSubmitInfo submit{};
    submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pCommandBuffers    = &commandBuffer.current();
    submit.commandBufferCount = 1;
    vkCheck(vkResetFences(vulkanState.device, 1, &fence.current()));
    vkCheck(vkQueueSubmit(vulkanState.graphicsQueue, 1, &submit, fence.current()));

    // cleanup
    memoryBarriers.clear();
    bufferBarriers.clear();
    imageBarriers.clear();
}

void TransferEngine::Bucket::resetResourcesWithSync() {
    vkCheck(vkWaitForFences(vulkanState.device, 1, &fence.current(), VK_TRUE, UINT64_MAX));
    vkCheck(vkResetCommandBuffer(commandBuffer.current(), 0));

    for (unsigned int i = 0; i < stagingBuffers.current().size(); ++i) {
        vmaDestroyBuffer(
            vulkanState.vmaAllocator, stagingBuffers.current()[i], stagingAllocs.current()[i]);
    }
    stagingBuffers.current().clear();
    stagingAllocs.current().clear();
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
} // namespace gfx
} // namespace bl

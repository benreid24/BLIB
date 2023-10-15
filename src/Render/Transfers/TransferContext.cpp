#include <BLIB/Render/Transfers/TransferContext.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace tfr
{
TransferContext::TransferContext(std::mutex& bucketMutex, vk::VulkanState& vs,
                                 std::vector<VkBuffer>& stagingBuffers,
                                 std::vector<VmaAllocation>& stagingAllocs,
                                 std::vector<VkMemoryBarrier>& memoryBarriers,
                                 std::vector<VkBufferMemoryBarrier>& bufferBarriers,
                                 std::vector<VkImageMemoryBarrier>& imageBarriers)
: bucketMutex(bucketMutex)
, vulkanState(vs)
, stagingBuffers(stagingBuffers)
, stagingAllocs(stagingAllocs)
, memoryBarriers(memoryBarriers)
, bufferBarriers(bufferBarriers)
, imageBarriers(imageBarriers) {}

void TransferContext::createTemporaryStagingBuffer(VkDeviceSize size, VkBuffer& bufferHandle,
                                                   void** mapped) {
    VmaAllocation alloc;
    VmaAllocationInfo allocInfo;
    vulkanState.createBuffer(
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &bufferHandle,
        &alloc,
        &allocInfo);

    std::unique_lock lock(bucketMutex);
    stagingBuffers.emplace_back(bufferHandle);
    stagingAllocs.emplace_back(alloc);
    *mapped = allocInfo.pMappedData;
}

void TransferContext::registerMemoryBarrier(const VkMemoryBarrier& barrier) {
    std::unique_lock lock(bucketMutex);
    memoryBarriers.emplace_back(barrier);
}

void TransferContext::registerBufferBarrier(const VkBufferMemoryBarrier& barrier) {
    std::unique_lock lock(bucketMutex);
    bufferBarriers.emplace_back(barrier);
}

void TransferContext::registerImageBarrier(const VkImageMemoryBarrier& barrier) {
    std::unique_lock lock(bucketMutex);
    imageBarriers.emplace_back(barrier);
}

VkDevice TransferContext::device() const { return vulkanState.device; }

} // namespace tfr
} // namespace rc
} // namespace bl

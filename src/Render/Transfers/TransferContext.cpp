#include <BLIB/Render/Transfers/TransferContext.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
namespace tfr
{
TransferContext::TransferContext(VkDevice dev, std::vector<VkBuffer>& stagingBuffers,
                                 std::vector<VkDeviceMemory>& stagingMemory,
                                 std::vector<VkMemoryBarrier>& memoryBarriers,
                                 std::vector<VkBufferMemoryBarrier>& bufferBarriers,
                                 std::vector<VkImageMemoryBarrier>& imageBarriers)
: vkdev(dev)
, stagingBuffers(stagingBuffers)
, stagingMemory(stagingMemory)
, memoryBarriers(memoryBarriers)
, bufferBarriers(bufferBarriers)
, imageBarriers(imageBarriers) {}

void TransferContext::createTemporaryStagingBuffer(VkDeviceSize size, VkBuffer& bufferHandle,
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

void TransferContext::registerMemoryBarrier(const VkMemoryBarrier& barrier) {
    memoryBarriers.emplace_back(barrier);
}

void TransferContext::registerBufferBarrier(const VkBufferMemoryBarrier& barrier) {
    bufferBarriers.emplace_back(barrier);
}

void TransferContext::registerImageBarrier(const VkImageMemoryBarrier& barrier) {
    imageBarriers.emplace_back(barrier);
}

} // namespace tfr
} // namespace render
} // namespace bl

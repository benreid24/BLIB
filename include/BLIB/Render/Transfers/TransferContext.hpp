#ifndef BLIB_RENDER_TRANSFERS_TRANSFERCONTEXT_HPP
#define BLIB_RENDER_TRANSFERS_TRANSFERCONTEXT_HPP

#include <glad/vulkan.h>
#include <vector>
#include <vk_mem_alloc.h>

namespace bl
{
namespace gfx
{
namespace vk
{
struct VulkanState;
}

namespace tfr
{
class TransferContext {
public:
    /**
     * @brief Creates a staging buffer of the given size. Should only be called from
     *        Transferable::executeTransferAndInsertBarriers. Staging buffers are freed at the end
     *        of the frame after queue submission
     *
     * @param size Size of the staging buffer to create
     * @param bufferResult Buffer handle to populate
     * @param mappedMemory Pointer to a void pointer which will be populated with the write address
     */
    void createTemporaryStagingBuffer(VkDeviceSize size, VkBuffer& bufferResult,
                                      void** mappedMemory);

    /**
     * @brief Registers the given barrier to be recorded after all transfers are started
     *
     * @param barrier The barrier to add
     */
    void registerMemoryBarrier(const VkMemoryBarrier& barrier);

    /**
     * @brief Registers the given barrier to be recorded after all transfers are started
     *
     * @param barrier The barrier to add
     */
    void registerBufferBarrier(const VkBufferMemoryBarrier& barrier);

    /**
     * @brief Registers the given barrier to be recorded after all transfers are started
     *
     * @param barrier The barrier to add
     */
    void registerImageBarrier(const VkImageMemoryBarrier& barrier);

    /**
     * @brief Returns the logical device to be used
     */
    VkDevice device() const;

    /**
     * @brief Internal constructor, do not use
     */
    TransferContext(vk::VulkanState& vulkanState, std::vector<VkBuffer>& stagingBuffers,
                    std::vector<VmaAllocation>& stagingAllocs,
                    std::vector<VkMemoryBarrier>& memoryBarriers,
                    std::vector<VkBufferMemoryBarrier>& bufferBarriers,
                    std::vector<VkImageMemoryBarrier>& imageBarriers);

private:
    vk::VulkanState& vulkanState;
    std::vector<VkBuffer>& stagingBuffers;
    std::vector<VmaAllocation>& stagingAllocs;
    std::vector<VkMemoryBarrier>& memoryBarriers;
    std::vector<VkBufferMemoryBarrier>& bufferBarriers;
    std::vector<VkImageMemoryBarrier>& imageBarriers;
};

} // namespace tfr
} // namespace gfx
} // namespace bl

#endif

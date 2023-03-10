#ifndef BLIB_RENDER_TRANSFERS_TRANSFERCONTEXT_HPP
#define BLIB_RENDER_TRANSFERS_TRANSFERCONTEXT_HPP

#include <glad/vulkan.h>
#include <vector>

namespace bl
{
namespace render
{
namespace tfr
{
class TransferEngine;
class TransferEngine::Bucket;

class TransferContext {
public:
    /**
     * @brief Creates a staging buffer of the given size. Should only be called from
     *        Transferable::executeTransferAndInsertBarriers. Staging buffers are freed at the end
     *        of the frame after queue submission
     *
     * @param size Size of the staging buffer to create
     * @param bufferResult Buffer handle to populate
     * @param memoryResult Memory handle to populate
     */
    void createTemporaryStagingBuffer(VkDeviceSize size, VkBuffer& bufferResult,
                                      VkDeviceMemory& memoryResult);
    // TODO - provide persistent buffers here too?
    // TODO - prepass to determine stage buf reqs and create one big one?

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
    constexpr VkDevice device() const;

private:
    const VkDevice vkdev;
    std::vector<VkBuffer>& stagingBuffers;
    std::vector<VkDeviceMemory>& stagingMemory;
    std::vector<VkMemoryBarrier>& memoryBarriers;
    std::vector<VkBufferMemoryBarrier>& bufferBarriers;
    std::vector<VkImageMemoryBarrier>& imageBarriers;

    TransferContext(VkDevice dev, std::vector<VkBuffer>& stagingBuffers,
                    std::vector<VkDeviceMemory>& stagingMemory,
                    std::vector<VkMemoryBarrier>& memoryBarriers,
                    std::vector<VkBufferMemoryBarrier>& bufferBarriers,
                    std::vector<VkImageMemoryBarrier>& imageBarriers);

    friend class TransferEngine::Bucket;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkDevice TransferContext::device() const { return vkdev; }

} // namespace tfr
} // namespace render
} // namespace bl

#endif

#ifndef BLIB_RENDER_VULKAN_TRANSFERENGINE_HPP
#define BLIB_RENDER_VULKAN_TRANSFERENGINE_HPP

#include <BLIB/Render/Vulkan/Transferable.hpp>
#include <glad/vulkan.h>
#include <mutex>
#include <vector>

namespace bl
{
namespace render
{
/**
 * @brief Utility class to manage the synchornized transfer of data to the GPU
 *
 * @ingroup Renderer
 */
class TransferEngine {
public:
    /**
     * @brief Queues the given item to be transfered on the next frame
     *
     * @param item The item to queue. Must remain valid until drained
     */
    void queueTransfer(Transferable& item);

    /**
     * @brief Creates a staging buffer of the given size. Should only be called from
     *        Transferable::executeTransferAndInsertBarriers. Staging buffers are freed at the end
     *        of the frame after queue submission
     * 
     * @param size Size of the staging buffer to create
     * @param bufferResult Buffer handle to populate
     * @param memoryResult Memory handle to populate
     */
    void createStagingBuffer(VkDeviceSize size, VkBuffer& bufferResult, VkDeviceMemory& memoryResult);

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

private:
    VulkanState& vulkanState;
    std::mutex mutex;
    std::vector<Transferable*> queuedItems;

    VkFence fence;
    std::vector<VkBuffer> stagingBuffers;
    std::vector<VkDeviceMemory> stagingMemory;

    std::vector<VkMemoryBarrier> memoryBarriers;
    std::vector<VkBufferMemoryBarrier> bufferBarriers;
    std::vector<VkImageMemoryBarrier> imageBarriers;

    TransferEngine(VulkanState& vulkanState);
    void init();
    void cleanup();

    void releaseStagingBuffers();
    void executeTransfers();

    friend class Renderer;
    friend struct VulkanState;
};

} // namespace render
} // namespace bl

#endif

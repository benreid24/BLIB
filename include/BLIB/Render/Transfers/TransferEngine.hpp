#ifndef BLIB_RENDER_VULKAN_TRANSFERENGINE_HPP
#define BLIB_RENDER_VULKAN_TRANSFERENGINE_HPP

#include <BLIB/Render/Transfers/TransferContext.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <glad/vulkan.h>
#include <mutex>
#include <vector>

namespace bl
{
namespace render
{
namespace tfr
{
/**
 * @brief Utility class to manage the synchronized transfer of data to the GPU
 *
 * @ingroup Renderer
 */
class TransferEngine {
public:
    /**
     * @brief Executes the queued transfers, potentially blocking on the prior set of transfers if
     *        not yet complete. This is called automatically each frame, but may be called as needed
     */
    void executeTransfers();

private:
    struct Bucket {
        VulkanState& vulkanState;
        VkCommandBuffer commandBuffer;
        VkFence fence;
        std::vector<VkBuffer> stagingBuffers;
        std::vector<VkDeviceMemory> stagingMemory;
        std::vector<VkMemoryBarrier> memoryBarriers; // TODO - bucket by dest stage?
        std::vector<VkBufferMemoryBarrier> bufferBarriers;
        std::vector<VkImageMemoryBarrier> imageBarriers;
        std::vector<Transferable*> oneTimeItems;
        std::vector<Transferable*> everyFrameItems;

        Bucket(VulkanState& vs);
        void init();
        void cleanup();
        constexpr bool hasTransfers() const;
        void executeTransfers();
        void resetResourcesWithSync();
    };

    VulkanState& vulkanState;
    std::mutex mutex;
    Bucket immediateBucket;
    Bucket frameBucket;

    TransferEngine(VulkanState& vulkanState);
    void init();
    void cleanup();

    // for use by Transferable
    void queueOneTimeTransfer(Transferable* item, Transferable::SyncRequirement syncReq);
    void registerPerFrameTransfer(Transferable* item, Transferable::SyncRequirement syncReq);
    void unregisterPerFrameTransfer(Transferable* item, Transferable::SyncRequirement syncReq);

    friend class Renderer;
    friend struct VulkanState;
    friend class Transferable;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr bool TransferEngine::Bucket::hasTransfers() const {
    return !everyFrameItems.empty() || !oneTimeItems.empty();
}

} // namespace tfr
} // namespace render
} // namespace bl

#endif

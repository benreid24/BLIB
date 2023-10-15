#ifndef BLIB_RENDER_VULKAN_TRANSFERENGINE_HPP
#define BLIB_RENDER_VULKAN_TRANSFERENGINE_HPP

#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Render/Transfers/TransferContext.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Vulkan.hpp>
#include <mutex>
#include <vector>

namespace bl
{
namespace rc
{
class Renderer;

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

    /**
     * @brief Creates CPU storage for vertices for a single frame. Useful for firing data to GPU
     *        that does not otherwise need to be kept around. Returned storage is invalidated after
     *        executeTransfers() is called
     *
     * @param count The number of vertices to create
     * @return A pointer to the beginning of the temporary vertices
     */
    prim::Vertex* createOneTimeVertexStorage(std::uint32_t count);

    /**
     * @brief Creates CPU storage for indices for a single frame. Useful for firing data to GPU
     *        that does not otherwise need to be kept around. Returned storage is invalidated after
     *        executeTransfers() is called
     *
     * @param count The number of indices to create
     * @return A pointer to the beginning of the temporary indices
     */
    std::uint32_t* createOneTimeIndexStorage(std::uint32_t count);

private:
    struct Bucket {
        std::mutex mutex;
        vk::VulkanState& vulkanState;
        vk::PerFrame<VkCommandBuffer> commandBuffer;
        vk::PerFrame<VkFence> fence;
        vk::PerFrame<std::vector<VkBuffer>> stagingBuffers;
        vk::PerFrame<std::vector<VmaAllocation>> stagingAllocs;
        std::vector<VkMemoryBarrier> memoryBarriers;
        std::vector<VkBufferMemoryBarrier> bufferBarriers;
        std::vector<VkImageMemoryBarrier> imageBarriers;
        std::vector<Transferable*> oneTimeItems;
        std::vector<Transferable*> everyFrameItems;

        Bucket(vk::VulkanState& vs);
        void init();
        void cleanup();
        bool hasTransfers() const;
        void executeTransfers();
        void resetResourcesWithSync();
    };

    vk::VulkanState& vulkanState;
    std::mutex mutex;
    Bucket immediateBucket;
    Bucket frameBucket;

    std::vector<prim::Vertex> tempVertices;
    std::vector<std::uint32_t> tempIndices;

    TransferEngine(vk::VulkanState& vulkanState);
    void init();
    void cleanup();

    // for use by Transferable
    void queueOneTimeTransfer(Transferable* item, Transferable::SyncRequirement syncReq);
    void registerPerFrameTransfer(Transferable* item, Transferable::SyncRequirement syncReq);
    void unregisterPerFrameTransfer(Transferable* item, Transferable::SyncRequirement syncReq);

    friend class bl::rc::Renderer;
    friend struct vk::VulkanState;
    friend class Transferable;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline bool TransferEngine::Bucket::hasTransfers() const {
    return !everyFrameItems.empty() || !oneTimeItems.empty();
}

} // namespace tfr
} // namespace rc
} // namespace bl

#endif

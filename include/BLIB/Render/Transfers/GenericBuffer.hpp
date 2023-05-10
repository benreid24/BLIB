#ifndef BLIB_RENDER_TRANSFERS_GENERICBUFFER_HPP
#define BLIB_RENDER_TRANSFERS_GENERICBUFFER_HPP

#include <BLIB/Render/Transfers/GenericBufferPriv.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <glad/vulkan.h>
#include <type_traits>

namespace bl
{
namespace render
{
struct VulkanState;

namespace tfr
{
/**
 * @brief Represents a Vulkan buffer. Supports double-buffering and stage buffering under the hood
 *        while providing a single interface
 *
 * @tparam Memory The type of memory for the underlying buffer
 * @tparam DoubleBuffer Whether or not the buffer should be double buffered
 * @ingroup Renderer
 */
template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
class GenericBuffer : public Transferable {
public:
    /// Whether or not the buffer requires a staging buffer for transfers
    static constexpr bool StageRequired = (Memory & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0;

    /**
     * @brief Creates an empty buffer
     */
    GenericBuffer();

    /**
     * @brief Destroys the buffer and frees resources
     */
    virtual ~GenericBuffer();

    /**
     * @brief Destroys the existing buffer, if any, then creates a new one on both the CPU and GPU.
     *        Does not initialize data on either device
     *
     * @param vulkanState The Vulkan state of the renderer
     * @param size The number of bytes in the buffer
     * @param usage How the buffer will be used
     */
    void create(VulkanState& vulkanState, std::uint32_t size, VkBufferUsageFlags usage);

    /**
     * @brief Destroys the buffer and frees resources
     */
    void destroy();

    /**
     * @brief Queues the given data to be written to the buffer. Pointer must stay valid. Writes are
     *        performed by the TransferEngine on frame start. Call one of the queue methods on
     *        Transferable to configure the buffer to transfer as needed
     *
     * @param data Pointer to the data to configureWrite
     * @param offset Offset into the buffer to configureWrite at in terms of T indices
     * @param len Number of elements to configureWrite
     */
    void configureWrite(const void* data, std::uint32_t offset, std::uint32_t len);

    /**
     * @brief Returns the Vulkan handle to the GPU buffer
     */
    constexpr VkBuffer handle() const;

    /**
     * @brief Returns the number of elements in the buffer
     */
    constexpr std::uint32_t size() const;

    /**
     * @brief Returns the PerFrame buffer handles for double buffers
     */
    template<typename = typename std::enable_if<DoubleBuffer, PerFrame<VkBuffer>&>::type>
    constexpr PerFrame<VkBuffer>& handles() {
        return storage.buffers;
    }

    /**
     * @brief Transfers queued writes to the buffer
     *
     * @param commandBuffer Command buffer to record into
     * @param context Transfer context to use
     */
    virtual void executeTransfer(VkCommandBuffer commandBuffer, TransferContext& context) override;

private:
    priv::GenericBufferStorage<DoubleBuffer, StageRequired> storage;
    std::uint32_t len;
    const void* queuedWrite;
    std::uint32_t writeOff;
    std::uint32_t writeLen;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
GenericBuffer<Memory, DoubleBuffer>::GenericBuffer()
: Transferable()
, len(0) {}

template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
GenericBuffer<Memory, DoubleBuffer>::~GenericBuffer() {
    if (len > 0) { destroy(); }
}

template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
void GenericBuffer<Memory, DoubleBuffer>::create(VulkanState& vs, std::uint32_t bytes,
                                                 VkBufferUsageFlags usage) {
    if (vulkanState) { destroy(); }
    len         = bytes;
    vulkanState = &vs;
    storage.create(vs, len, Memory, usage);
}

template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
void GenericBuffer<Memory, DoubleBuffer>::destroy() {
    storage.destroy(*vulkanState);
    len = 0;
}

template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
void GenericBuffer<Memory, DoubleBuffer>::configureWrite(const void* data, std::uint32_t offset,
                                                         std::uint32_t len) {
    queuedWrite = data;
    writeOff    = offset;
    writeLen    = len;
}

template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
constexpr VkBuffer GenericBuffer<Memory, DoubleBuffer>::handle() const {
    return storage.current();
}

template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
constexpr std::uint32_t GenericBuffer<Memory, DoubleBuffer>::size() const {
    return len;
}

template<VkMemoryPropertyFlags Memory, bool DoubleBuffer>
void GenericBuffer<Memory, DoubleBuffer>::executeTransfer(VkCommandBuffer cb,
                                                          TransferContext& ctx) {
    if (queuedWrite) {
        storage.doWrite(cb, ctx, queuedWrite, writeOff, writeLen);

        VkBufferMemoryBarrier barrier{};
        barrier.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
        barrier.buffer              = storage.current();
        barrier.offset              = writeOff;
        barrier.size                = writeLen;
        ctx.registerBufferBarrier(barrier);
    }
}

} // namespace tfr
} // namespace render
} // namespace bl

#endif

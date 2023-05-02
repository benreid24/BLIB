#ifndef BLIB_RENDER_PRIMITIVES_GENERICBUFFER_HPP
#define BLIB_RENDER_PRIMITIVES_GENERICBUFFER_HPP

#include <BLIB/Render/Primitives/GenericBufferPriv.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <glad/vulkan.h>
#include <type_traits>

namespace bl
{
namespace render
{
struct VulkanState;

/// Collection of renderer primitives
namespace prim
{
template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
class GenericBuffer : public tfr::Transferable {
    static constexpr bool StageRequired = Memory & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT != 0;

public:
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
     * @param size The number of elements in the buffer
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
     * @param data Pointer to the data to write
     * @param offset Offset into the buffer to write at in terms of T indices
     * @param len Number of elements to write
     */
    void write(const T* data, std::uint32_t offset, std::uint32_t len);

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

protected:
    /**
     * @brief Transfers queued writes to the buffer
     *
     * @param commandBuffer Command buffer to record into
     * @param context Transfer context to use
     */
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& context) override;

private:
    priv::GenericBufferStorage<DoubleBuffer, StageRequired> storage;
    std::uint32_t len;
    const T* queuedWrite;
    std::uint32_t writeOff;
    std::uint32_t writeLen;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
GenericBuffer<T, Memory, DoubleBuffer>::GenericBuffer()
: Transferable() {}

template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
GenericBuffer<T, Memory, DoubleBuffer>::~GenericBuffer() {
    if (vulkanState) { destroy(); }
}

template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
void GenericBuffer<T, Memory, DoubleBuffer>::create(VulkanState& vs, std::uint32_t len,
                                                    VkBufferUsageFlags usage) {
    if (vulkanState) { destroy(); }
    vulkanState = &vs;
    storage.create(vs, len * sizeof(T), Memory, usage);
}

template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
void GenericBuffer<T, Memory, DoubleBuffer>::destroy() {
    storage.destroy(*vulkanState);
    vulkanState = nullptr;
}

template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
void GenericBuffer<T, Memory, DoubleBuffer>::write(const T* data, std::uint32_t offset,
                                                   std::uint32_t len) {
    queuedWrite = data;
    writeOff    = offset * sizeof(T);
    writeLen    = len * sizeof(T);
}

template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
constexpr VkBuffer GenericBuffer<T, Memory, DoubleBuffer>::handle() const {
    return storage.current();
}

template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
constexpr std::uint32_t GenericBuffer<T, Memory, DoubleBuffer>::size() const {
    return len;
}

template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
void GenericBuffer<T, Memory, DoubleBuffer>::executeTransfer(VkCommandBuffer cb,
                                                             tfr::TransferContext& ctx) {
    if (queuedWrite) {
        storage.doWrite(cb, ctx, queuedWrite, writeOff, writeLen);
        queuedWrite = nullptr; // TODO - maybe keep this for per-frame writes?

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

} // namespace prim
} // namespace render
} // namespace bl

#endif

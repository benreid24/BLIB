#ifndef BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFERDOUBLESTAGED_HPP
#define BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFERDOUBLESTAGED_HPP

#include <BLIB/Render/Buffers/Interfaces/BindableBufferDouble.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
namespace base
{
/**
 * @brief Double buffered buffer class that provides dual device local buffers and dual persistent
 *        staging buffers
 *
 * @tparam T The payload type that the buffer will hold
 * @tparam Align The alignment to use if storing more than one element
 * @tparam Usage The usage type to create the buffers with
 * @tparam MemoryPool The type of memory to allocate the buffer in
 * @tparam AllocFlags Allocation flags to forward to VMA
 * @tparam FallbackMemoryPool Memory pool to use if primary allocation fails
 * @ingroup Renderer
 */
template<typename T, Alignment Align, VkBufferUsageFlags Usage, VkMemoryPropertyFlags MemoryPool,
         VmaAllocationCreateFlags AllocFlags      = 0,
         VkMemoryPropertyFlags FallbackMemoryPool = MemoryPool>
class BindableBufferDoubleStaged : public BindableBuffer<T, Align> {
public:
    /// Type signature for the device local buffers
    using TDeviceBuffers = BindableBufferDouble<T, Align, Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                MemoryPool, AllocFlags, FallbackMemoryPool>;

    /// Type signature for the staging buffers
    using TStagingBuffers = BindableBufferDouble<
        T, Align, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT>;

    /**
     * @brief Creates the buffer
     */
    BindableBufferDoubleStaged() = default;

    /**
     * @brief Destroys the buffers
     */
    virtual ~BindableBufferDoubleStaged() = default;

    /**
     * @brief Resizes the buffers
     *
     * @param size The new number of data elements to size for
     */
    virtual void resize(std::uint32_t size) override {
        stagingBuffers.resize(size);
        deviceBuffers.resize(size);
    }

    /**
     * @brief Immediately destroys the buffers
     */
    virtual void destroy() override {
        stagingBuffers.destroy();
        deviceBuffers.destroy();
    }

    /**
     * @brief Queues the buffers to be destroyed after MaxConcurrentFrames
     */
    virtual void deferDestroy() override {
        stagingBuffers.deferDestroy();
        deviceBuffers.deferDestroy();
    }

    /**
     * @brief Returns the underlying buffer for the given frame index
     *
     * @param frameIndex The frame index to get the buffer for
     */
    virtual vk::Buffer& getBuffer(std::uint32_t frameIndex) override {
        return deviceBuffers.getBuffer(frameIndex);
    }

    /**
     * @brief Returns the number of elements currently in the buffer
     */
    virtual std::uint32_t getSize() const override { return deviceBuffers.getSize(); }

    /**
     * @brief Fills the current frame buffer with the given value. Only call if the buffers are CPU
     *        visible and mapped
     *
     * @tparam U Some type that can be assigned to T. Overload operator= if required
     * @param value The value to copy into all existing slots
     */
    template<typename U>
    void fillDirect(const U& value) {
        stagingBuffers.template fillDirect<U>(value);
    }

    /**
     * @brief Writes directly to the current frame buffer from the given source. Only call if the
     *        buffers are CPU visible and mapped
     *
     * @tparam U Some type that can be assigned to T. Overload operator= if required
     * @param base The first element to copy from
     * @param len The number of elements to copy
     * @param start The index to start writing at
     */
    template<typename U>
    void writeDirect(U* base, std::size_t len, std::uint32_t start = 0) {
        stagingBuffers.template writeDirect<U>(base, len, start);
    }

    /**
     * @brief Returns the address to write to the given element in the current staging buffer
     *
     * @param i The index to write to
     * @return The address of the data at the given index
     */
    T* getWriteAddress(std::uint32_t i = 0) { return stagingBuffers.getWriteAddress(i); }

    /**
     * @brief Returns the staging buffers
     */
    TStagingBuffers& getStagingBuffers() { return stagingBuffers; }

    /**
     * @brief Returns the device local buffers
     */
    TDeviceBuffers& getDeviceBuffers() { return deviceBuffers; }

protected:
    TStagingBuffers stagingBuffers;
    TDeviceBuffers deviceBuffers;

    /**
     * @brief Creates the buffers. Should be called by derived classes if they override it
     *
     * @param renderer The renderer instance
     * @param n The number of elements to size the buffer for
     */
    virtual void doCreate(Renderer& renderer, std::uint32_t n) override {
        deviceBuffers.create(renderer, n);
        stagingBuffers.create(renderer, n);
    }

    /**
     * @brief Copies the full staging buffer to the device buffer for the current frame
     *
     * @param commandBuffer The command buffer to record into
     * @param ctx The transfer context
     */
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& ctx) override {
        const VkDeviceSize copySize = this->getTotalAlignedSize();

        VkBufferCopy copyCmd{};
        copyCmd.dstOffset = 0;
        copyCmd.size      = copySize;
        copyCmd.srcOffset = 0;
        vkCmdCopyBuffer(commandBuffer,
                        stagingBuffers.getCurrentFrameRawBuffer(),
                        deviceBuffers.getCurrentFrameRawBuffer(),
                        1,
                        &copyCmd);

        VkBufferMemoryBarrier bufBarrier{};
        bufBarrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        bufBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        bufBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        bufBarrier.buffer        = deviceBuffers.getCurrentFrameRawBuffer();
        bufBarrier.offset        = copyCmd.dstOffset;
        bufBarrier.size          = copyCmd.size;
        ctx.registerBufferBarrier(bufBarrier);
    }
};

} // namespace base
} // namespace buf
} // namespace rc
} // namespace bl

#endif

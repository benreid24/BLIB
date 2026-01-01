#ifndef BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFERSINGLESOURCED_HPP
#define BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFERSINGLESOURCED_HPP

#include <BLIB/Render/Buffers/AlignedBuffer.hpp>
#include <BLIB/Render/Buffers/Interfaces/BindableBufferSingle.hpp>
#include <BLIB/Util/VectorRef.hpp>

namespace bl
{
namespace rc
{

namespace buf
{
namespace base
{
/**
 * @brief Single buffered buffer class that provides buffer management and direct access for mapped
 *        buffers that are CPU visible. Also provides a host buffer to store the contained payloads
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
class BindableBufferSingleSourced
: public BindableBufferSingle<T, Align, Usage, MemoryPool, AllocFlags, FallbackMemoryPool> {
    using Base = BindableBufferSingle<T, Align, Usage, MemoryPool, AllocFlags, FallbackMemoryPool>;

public:
    /**
     * @brief Creates the buffe
     */
    BindableBufferSingleSourced() = default;

    /**
     * @brief Destroys the buffer
     */
    virtual ~BindableBufferSingleSourced() = default;

    /**
     * @brief Resizes the buffers
     *
     * @param size The new number of data elements to size for
     */
    virtual void resize(std::uint32_t size) {
        sourceBuffer.resize(size);
        Base::resize(size);
    }

    /**
     * @brief Fills the buffer with the given value
     *
     * @param value The value to copy into all existing slots
     */
    void fill(const T& value) { sourceBuffer.fill(value); }

    /**
     * @brief Returns the element at the given index
     *
     * @param i Index of the element to access. Not bounds checked
     * @return A reference to the element at the given index
     */
    T& operator[](std::uint32_t i) { return sourceBuffer[i]; }

    /**
     * @brief Returns the element at the given index
     *
     * @param i Index of the element to access. Not bounds checked
     * @return A reference to the element at the given index
     */
    const T& operator[](std::uint32_t i) const { return sourceBuffer[i]; }

    /**
     * @brief Assigns the stable reference to the CPU side storage of this SSBO
     *
     * @param ref The reference to populate
     * @param i The object index to assign
     */
    void assignRef(util::VectorRef<T, buf::AlignedBuffer<T>>& ref, std::uint32_t i) {
        ref.assign(sourceBuffer, i);
    }

protected:
    AlignedBuffer<T> sourceBuffer;

    /**
     * @brief Creates the buffers. Should be called by derived classes if they override it
     *
     * @param vulkanState The renderer Vulkan state
     * @param n The number of elements to size the buffer for
     */
    virtual void doCreate(vk::VulkanLayer& vulkanState, std::uint32_t n) {
        sourceBuffer.create(Align, n);
        Base::doCreate(vulkanState, n);
    }

    /**
     * @brief Transfers the accumulated dirty range to the current buffer. Creates a staging buffer
     *        if the data cannot be directly written
     *
     * @param commandBuffer The command buffer to record into
     * @param context The transfer context
     */
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& context) override {
        if (this->isDirectWritable()) {
            const DirtyRange range = this->getCurrentDirtyRange();
            if (range.size > 0) {
                this->writeDirect(&sourceBuffer[range.start], range.size, range.start);
            }
            this->markClean();
        }
        else {
            const DirtyRange range = this->getCurrentDirtyRange();
            if (range.size > 0) {
                const VkDeviceSize copySize = range.size * this->getAlignedElementSize();
                VkBuffer stagingBuffer;
                void* stagingMem;
                context.createTemporaryStagingBuffer(copySize, stagingBuffer, &stagingMem);
                std::memcpy(stagingMem, &sourceBuffer[range.start], copySize);

                VkBufferCopy copyCmd{};
                copyCmd.dstOffset = range.start * this->getAlignedElementSize();
                copyCmd.size      = copySize;
                copyCmd.srcOffset = 0;
                vkCmdCopyBuffer(
                    commandBuffer, stagingBuffer, this->getCurrentFrameRawBuffer(), 1, &copyCmd);

                VkBufferMemoryBarrier bufBarrier{};
                bufBarrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                bufBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                bufBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                bufBarrier.buffer        = this->getCurrentFrameRawBuffer();
                bufBarrier.offset        = copyCmd.dstOffset;
                bufBarrier.size          = copyCmd.size;
                context.registerBufferBarrier(bufBarrier);
            }
            this->markClean();
        }
    }
};

} // namespace base
} // namespace buf
} // namespace rc
} // namespace bl

#endif

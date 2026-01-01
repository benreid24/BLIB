#ifndef BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFERSINGLE_HPP
#define BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFERSINGLE_HPP

#include <BLIB/Render/Buffers/Interfaces/BindableBuffer.hpp>

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
 *        buffers that are CPU visible
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
class BindableBufferSingle : public BindableBuffer<T, Align> {
public:
    /// Represents whether the buffer is persistently mapped or not
    static constexpr bool IsMapped = (AllocFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;

    /// Represents whether the buffer may be directly written to or requires a staging buffer
    static constexpr bool IsDirectWritable =
        (MemoryPool & FallbackMemoryPool & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0 && IsMapped;

    /**
     * @brief Creates the buffer
     */
    BindableBufferSingle()
    : numElements(0) {}

    /**
     * @brief Destroys the buffer
     */
    virtual ~BindableBufferSingle() = default;

    /**
     * @brief Resizes the buffer
     *
     * @param size The new number of data elements to size for
     */
    virtual void resize(std::uint32_t size) override {
        numElements = size;
        buffer.ensureSize(this->getTotalAlignedSize(), false);
    }

    /**
     * @brief Immediately destroys the buffers
     */
    virtual void destroy() override { buffer.destroy(); }

    /**
     * @brief Queues the buffers to be destroyed after MaxConcurrentFrames
     */
    virtual void deferDestroy() override { buffer.deferDestruction(); }

    /**
     * @brief Returns the underlying buffer for the given frame index
     *
     * @param Unused
     */
    virtual vk::Buffer& getBuffer(std::uint32_t) override { return buffer; }

    /**
     * @brief Returns the number of elements currently in the buffer
     */
    virtual std::uint32_t getSize() const override { return numElements; }

    /**
     * @brief Fills the buffer with the given value. Only call if the buffers are CPU
     *        visible and mapped
     *
     * @tparam U Some type that can be assigned to T. Overload operator= if required
     * @param value The value to copy into all existing slots
     */
    template<typename U>
    void fillDirect(const U& value) {
        if (!isDirectWritable()) {
            BL_LOG_ERROR << "Cannot directly write to non-mapped non-visible buffer";
        }

        char* dst = static_cast<char*>(buffer.getMappedMemory());
        char* end = dst + buffer.getSize();
        while (dst != end) {
            T* slot = static_cast<T*>(static_cast<void*>(dst));
            *slot   = value;
            dst += this->getAlignedElementSize();
        }
    }

    /**
     * @brief Writes directly to the buffer from the given source. Only call if the
     *        buffer is CPU visible and mapped
     *
     * @tparam U Some type that can be assigned to T. Overload operator= if required
     * @param base The first element to copy from
     * @param len The number of elements to copy
     * @param start The index to start writing at
     */
    template<typename U>
    void writeDirect(U* base, std::size_t len, std::uint32_t start = 0) {
        if (!isDirectWritable()) {
            BL_LOG_ERROR << "Cannot directly write to non-mapped non-visible buffer";
        }

        U* end = base + len;
        char* dst =
            static_cast<char*>(buffer.getMappedMemory()) + start * this->getAlignedElementSize();
        while (base != end) {
            T* slot = static_cast<T*>(static_cast<void*>(dst));
            *slot   = *base;
            ++base;
            dst += this->getAlignedElementSize();
        }
    }

    /**
     * @brief Returns the address to write to the given element. Must only be called on writable
     *        buffers that are mapped on creation
     *
     * @param i The index to write to
     * @return The address of the data at the given index
     */
    T* getWriteAddress(std::uint32_t i) {
        if (!isDirectWritable()) {
            BL_LOG_ERROR << "Cannot directly write to non-mapped non-visible buffer";
        }

        char* raw =
            static_cast<char*>(buffer.getMappedMemory()) + i * this->getAlignedElementSize();
        return static_cast<T*>(static_cast<void*>(raw));
    }

    /**
     * @brief Returns whether the selected memory pool allows the buffers to be directly written to
     */
    bool isDirectWritable() const {
        return IsMapped && (buffer.getMemoryPool() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
    }

protected:
    vk::Buffer buffer;

    /**
     * @brief Creates the buffers. Should be called by derived classes if they override it
     *
     * @param vulkanState The renderer Vulkan state
     * @param n The number of elements to size the buffer for
     */
    virtual void doCreate(vk::VulkanLayer& vulkanState, std::uint32_t n) {
        numElements = n;
        buffer.createWithFallback(vulkanState,
                                  this->getTotalAlignedSize(),
                                  MemoryPool,
                                  FallbackMemoryPool,
                                  Usage,
                                  AllocFlags);
    }

private:
    std::uint32_t numElements;
};

} // namespace base
} // namespace buf
} // namespace rc
} // namespace bl

#endif

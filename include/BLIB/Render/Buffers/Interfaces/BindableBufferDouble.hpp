#ifndef BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFERDOUBLE_HPP
#define BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFERDOUBLE_HPP

#include <BLIB/Render/Buffers/Interfaces/BindableBuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
namespace base
{
/**
 * @brief Double buffered buffer class that provides buffer management and direct access for mapped
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
class BindableBufferDouble : public BindableBuffer<T, Align> {
public:
    /// Represents whether the buffer may be directly written to or requires a staging buffer
    static constexpr bool IsDirectWritable =
        (MemoryPool & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0 &&
        (AllocFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;

    /**
     * @brief Creates the buffer
     */
    BindableBufferDouble() = default;

    /**
     * @brief Destroys the buffers
     */
    virtual ~BindableBufferDouble() = default;

    /**
     * @brief Resizes the buffers
     *
     * @param size The new number of data elements to size for
     */
    virtual void resize(std::uint32_t size) {
        numElements = size;
        buffers.visit(
            [this, size](vk::Buffer& buffer) { buffer.ensureSize(getTotalAlignedSize(), false); });
    }

    /**
     * @brief Immediately destroys the buffers
     */
    virtual void destroy() override {
        buffers.cleanup([](vk::Buffer& buffer) { buffer.destroy(); });
    }

    /**
     * @brief Queues the buffers to be destroyed after MaxConcurrentFrames
     */
    virtual void deferDestroy() override {
        buffers.cleanup([](vk::Buffer& buffer) { buffer.deferDestruction(); });
    }

    /**
     * @brief Returns the underlying buffer for the given frame index
     *
     * @param frameIndex The frame index to get the buffer for
     */
    virtual vk::Buffer& getBuffer(std::uint32_t frameIndex) override { return buffers.getRaw(i); }

    /**
     * @brief Returns the number of elements currently in the buffer
     */
    virtual std::uint32_t getSize() const override { return numElements; }

    /**
     * @brief Fills the current frame buffer with the given value. Only call if the buffers are CPU
     *        visible and mapped
     *
     * @tparam U Some type that can be assigned to T. Overload operator= if required
     * @param value The value to copy into all existing slots
     */
    template<typename U>
    void fillDirect(const U& value) {
        static_assert(IsDirectWritable, "Buffer must be host visible and mapped for direct write");

        char* dst = static_cast<char*>(buffers.current().getMappedMemory());
        char* end = dst + buffers.current().getSize();
        while (dst != end) {
            T* slot = static_cast<T*>(static_cast<void*>(dst));
            *slot   = value;
            dst += getAlignedElementSize();
        }
    }

    /**
     * @brief Writes directly to the current frame buffer from the given source. Only call if the
     *        buffers are CPU visible and mapped
     *
     * @tparam U Some type that can be assigned to T. Overload operator= if required
     * @param base The first element to copy from
     * @param len The number of elements to copy
     * @param start The index to start writing at
     * @return True if the buffer grew, false otherwise
     */
    template<typename U>
    bool writeDirect(U* base, std::size_t len, std::uint32_t start = 0) {
        static_assert(IsDirectWritable, "Buffer must be host visible and mapped for direct write");

        U* end    = base + len;
        char* dst = static_cast<char*>(buffers.current().getMappedMemory()) +
                    start * getAlignedElementSize();
        while (base != end) {
            T* slot = static_cast<T*>(static_cast<void*>(dst));
            *slot   = *base;
            ++base;
            dst += getAlignedElementSize();
        }
    }

protected:
    vk::PerFrame<vk::Buffer> buffers;

    /**
     * @brief Creates the buffers. Should be called by derived classes if they override it
     *
     * @param vulkanState The renderer Vulkan state
     * @param n The number of elements to size the buffer for
     */
    virtual void doCreate(vk::VulkanState& vulkanState, std::uint32_t n) {
        numElements = n;
        buffers.init(vulkanState, [this, &vulkanState](vk::Buffer& buffer) {
            buffer.createWithFallback(vulkanState,
                                      getTotalAlignedSize(),
                                      MemoryPool,
                                      FallbackMemoryPool,
                                      Usage,
                                      AllocFlags);
        });
    }

private:
    std::uint32_t numElements;
};

} // namespace base
} // namespace buf
} // namespace rc
} // namespace bl

#endif

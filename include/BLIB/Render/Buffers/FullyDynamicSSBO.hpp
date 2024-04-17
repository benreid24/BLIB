#ifndef BLIB_RENDER_BUFFERS_FULLYDYNAMICSSBO_HPP
#define BLIB_RENDER_BUFFERS_FULLYDYNAMICSSBO_HPP

#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/AlignedBuffer.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Util/VectorRef.hpp>
#include <array>
#include <limits>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Device-local SSBO intended for use with per-object data for dynamic objects. This version
 *        of DynamicSSBO does not provide a CPU-side buffer and assumes that the user will fully
 *        rewrite the entire buffer every frame. If that is not the case then use DynamicSSBO
 *
 * @tparam T The type of data to pack into the SSBO
 * @ingroup Renderer
 */
template<typename T>
class FullyDynamicSSBO {
public:
    /**
     * @brief Creates a new SSBO
     */
    FullyDynamicSSBO() = default;

    /**
     * @brief Creates a new SSBO
     *
     * @param vulkanState Renderer Vulkan state
     * @param size Number of elements to create
     */
    FullyDynamicSSBO(vk::VulkanState& vulkanState, std::uint32_t size);

    /**
     * @brief Creates a new SSBO
     *
     * @param vulkanState Renderer Vulkan state
     * @param size Number of elements to create
     */
    void create(vk::VulkanState& vulkanState, std::uint32_t size);

    /**
     * @brief Fills the buffer with the given value
     *
     * @tparam U Some type that can be assigned to T. Overload operator= if required
     * @param value The value to copy into all existing slots
     */
    template<typename U>
    void fill(const U& value);

    /**
     * @brief If the buffer size is less than the required size, re-creates the buffer to be at
     *        least the given size
     *
     * @param size The size the buffer should be, at a minimum
     * @param skipCopy Optionally skip copying the old buffer to the new if it was resized
     * @return True if the buffer was resized, false if it was big enough
     */
    bool ensureSize(std::uint32_t newSize, bool skipCopy = false);

    /**
     * @brief Frees the GPU buffers
     */
    void destroy();

    /**
     * @brief Populates the current frame's buffer from the array of source elements. Will resize
     *        the buffer if required
     *
     * @tparam U Some type that can be assigned to T. Overload operator= if required
     * @param base The first element to copy from
     * @param len The number of elements to copy
     * @return True if the buffer grew, false otherwise
     */
    template<typename U>
    bool performFullCopy(U* base, std::size_t len);

    /**
     * @brief Returns the Buffer objects of the GPU buffers
     */
    vk::PerFrame<vk::Buffer>& gpuBufferHandles();

    /**
     * @brief Returns the size of the SSBO on the device
     */
    VkDeviceSize getTotalRange() const;

private:
    vk::VulkanState* vulkanState;
    vk::PerFrame<vk::Buffer> gpuBuffers;
    std::uint32_t alignment;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
FullyDynamicSSBO<T>::FullyDynamicSSBO(vk::VulkanState& vulkanState, std::uint32_t size) {
    create(vulkanState, size);
}

template<typename T>
void FullyDynamicSSBO<T>::create(vk::VulkanState& vs, std::uint32_t size) {
    vulkanState = &vs;
    alignment   = vs.computeAlignedSize(
        sizeof(T), vs.physicalDeviceProperties.limits.minStorageBufferOffsetAlignment);

    gpuBuffers.init(vs, [this, &vs, size](vk::Buffer& buffer) {
        buffer.createWithFallback(
            vs,
            alignment * size,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT);
    });
}

template<typename T>
template<typename U>
void FullyDynamicSSBO<T>::fill(const U& value) {
    char* dst = static_cast<char*>(gpuBuffers.current().getMappedMemory());
    char* end = dst + gpuBuffers.current().getSize();
    while (dst != end) {
        T* slot = static_cast<T*>(static_cast<void*>(dst));
        *slot   = *value;
        dst += alignment;
    }
}

template<typename T>
void FullyDynamicSSBO<T>::destroy() {
    gpuBuffers.cleanup([](vk::Buffer& buffer) { buffer.destroy(); });
}

template<typename T>
vk::PerFrame<vk::Buffer>& FullyDynamicSSBO<T>::gpuBufferHandles() {
    return gpuBuffers;
}

template<typename T>
VkDeviceSize FullyDynamicSSBO<T>::getTotalRange() const {
    return gpuBuffers.current().getSize();
}

template<typename T>
bool FullyDynamicSSBO<T>::ensureSize(std::uint32_t desiredSize, bool skipCopy) {
    const std::uint32_t requiredSize = desiredSize * alignment;

    if (requiredSize > gpuBuffers.current().getSize()) {
        std::uint32_t newSize = gpuBuffers.current().getSize() * 2;
        while (newSize < requiredSize) { newSize *= 2; }

        gpuBuffers.visit([this, newSize, skipCopy](vk::Buffer& buffer) {
            buffer.ensureSize(newSize, skipCopy);
        });

        return true;
    }
    return false;
}

template<typename T>
template<typename U>
bool FullyDynamicSSBO<T>::performFullCopy(U* base, std::size_t len) {
    const bool grew = ensureSize(len, true);
    U* end          = base + len;
    char* dst       = static_cast<char*>(gpuBuffers.current().getMappedMemory());
    while (base != end) {
        T* slot = static_cast<T*>(static_cast<void*>(dst));
        *slot   = *base;
        ++base;
        dst += alignment;
    }
    return grew;
}

} // namespace buf
} // namespace rc
} // namespace bl

#endif

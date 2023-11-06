#ifndef BLIB_RENDER_BUFFERS_DYNAMICSSBO_HPP
#define BLIB_RENDER_BUFFERS_DYNAMICSSBO_HPP

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
 * @brief Device-local SSBO intended for use with per-object data for dynamic objects
 *
 * @tparam T The type of data to pack into the SSBO
 * @ingroup Renderer
 */
template<typename T>
class DynamicSSBO {
public:
    /**
     * @brief Creates a new SSBO
     */
    DynamicSSBO() = default;

    /**
     * @brief Creates a new SSBO
     *
     * @param vulkanState Renderer Vulkan state
     * @param size Number of elements to create
     */
    DynamicSSBO(vk::VulkanState& vulkanState, std::uint32_t size);

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
     * @param value The value to copy into all existing slots
     */
    void fill(const T& value);

    /**
     * @brief Returns the element at the given index
     *
     * @param i Index of the element to access. Not bounds checked
     * @return A reference to the element at the given index
     */
    T& operator[](std::uint32_t i);

    /**
     * @brief Returns the element at the given index
     *
     * @param i Index of the element to access. Not bounds checked
     * @return A reference to the element at the given index
     */
    const T& operator[](std::uint32_t i) const;

    /**
     * @brief Assigns the stable reference to the CPU side storage of this SSBO
     *
     * @param ref The reference to populate
     * @param i The object index to assign
     */
    void assignRef(util::VectorRef<T, vk::AlignedBuffer<T>>& ref, std::uint32_t i);

    /**
     * @brief If the buffer size is less than the required size, re-creates the buffer to be at
     *        least the given size
     *
     * @param size The size the buffer should be, at a minimum
     * @return True if the buffer was resized, false if it was big enough
     */
    bool ensureSize(std::uint32_t newSize);

    /**
     * @brief Returns the number of elements in the buffer
     */
    constexpr std::uint32_t size() const;

    /**
     * @brief Frees the GPU buffers
     */
    void destroy();

    /**
     * @brief Returns the Buffer objects of the GPU buffers
     */
    constexpr vk::PerFrame<vk::Buffer>& gpuBufferHandles();

    /**
     * @brief Copies the given dirty range into the GPU buffer, as well as the prior dirty range
     *
     * @param start The first element to copy
     * @param numElements The number of elements to copy
     */
    void transferRange(std::uint32_t start, std::uint32_t numElements);

    /**
     * @brief Copies the entire buffer
     */
    void transferAll();

    /**
     * @brief Returns the size of the SSBO on the device
     */
    constexpr VkDeviceSize getTotalRange() const;

private:
    struct DirtyRange {
        std::uint32_t start;
        std::uint32_t size;

        DirtyRange()
        : start(0)
        , size(0) {}

        DirtyRange(std::uint32_t s, std::uint32_t si)
        : start(s)
        , size(si) {}
    };

    vk::VulkanState* vulkanState;
    vk::AlignedBuffer<T> cpuBuffer;
    vk::PerFrame<vk::Buffer> gpuBuffers;
    std::array<DirtyRange, Config::MaxConcurrentFrames> dirtyRanges;
    std::uint32_t dirtyThresh;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
DynamicSSBO<T>::DynamicSSBO(vk::VulkanState& vulkanState, std::uint32_t size) {
    create(vulkanState, size);
}

template<typename T>
void DynamicSSBO<T>::create(vk::VulkanState& vs, std::uint32_t size) {
    vulkanState = &vs;
    dirtyThresh = size * 7 / 10;

    cpuBuffer.create(vs, vk::AlignedBuffer<T>::StorageBuffer, size);
    gpuBuffers.init(vs, [&vs, this](vk::Buffer& buffer) {
        buffer.createWithFallback(
            vs,
            cpuBuffer.alignedSize(),
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT);
    });
}

template<typename T>
void DynamicSSBO<T>::fill(const T& value) {
    cpuBuffer.fill(value);
}

template<typename T>
T& DynamicSSBO<T>::operator[](std::uint32_t i) {
    return cpuBuffer[i];
}

template<typename T>
const T& DynamicSSBO<T>::operator[](std::uint32_t i) const {
    return cpuBuffer[i];
}
template<typename T>
void DynamicSSBO<T>::assignRef(util::VectorRef<T, vk::AlignedBuffer<T>>& ref, std::uint32_t i) {
    ref.assign(cpuBuffer, i);
}

template<typename T>
constexpr std::uint32_t DynamicSSBO<T>::size() const {
    return cpuBuffer.size();
}

template<typename T>
inline void DynamicSSBO<T>::destroy() {
    cpuBuffer.clear();
    gpuBuffers.cleanup([](vk::Buffer& buffer) { buffer.destroy(); });
}

template<typename T>
inline constexpr vk::PerFrame<vk::Buffer>& DynamicSSBO<T>::gpuBufferHandles() {
    return gpuBuffers;
}

template<typename T>
bool DynamicSSBO<T>::ensureSize(std::uint32_t desiredSize) {
    if (desiredSize > cpuBuffer.size()) {
        std::uint32_t newSize = cpuBuffer.size() * 2;
        while (newSize < desiredSize) { newSize *= 2; }

        dirtyThresh = newSize * 7 / 10;
        cpuBuffer.resize(newSize);
        gpuBuffers.visit(
            [this](vk::Buffer& buffer) { buffer.ensureSize(cpuBuffer.alignedSize()); });

        return true;
    }
    return false;
}

template<typename T>
void DynamicSSBO<T>::transferRange(std::uint32_t start, std::uint32_t numElements) {
    dirtyRanges[vulkanState->currentFrameIndex()] = DirtyRange{start, numElements};

    // determine if we should copy ranges or just copy entire buffer
    std::uint32_t totalSize = 0;
    for (auto& range : dirtyRanges) { totalSize += range.size; }
    if (totalSize >= dirtyThresh) {
        std::memcpy(
            gpuBuffers.current().getMappedMemory(), cpuBuffer.data(), cpuBuffer.alignedSize());
    }
    else if (totalSize > 0) {
        // consolidate ranges before copy
        std::uint32_t start = std::numeric_limits<std::uint32_t>::max();
        std::uint32_t end   = 0;
        for (auto& range : dirtyRanges) {
            if (range.size == 0) continue;
            const std::uint32_t ne = range.start + range.size;
            start                  = range.start < start ? range.start : start;
            end                    = ne > end ? ne : end;
        }
        if (end > start) {
            const std::uint32_t size = (end - start) * cpuBuffer.elementSize();
            void* dst                = static_cast<char*>(gpuBuffers.current().getMappedMemory()) +
                        (start * cpuBuffer.elementSize());
            std::memcpy(dst, &cpuBuffer[start], size);
        }
    }
}

template<typename T>
void DynamicSSBO<T>::transferAll() {
    dirtyRanges[vulkanState->currentFrameIndex()] = DirtyRange{0, size()};
    std::memcpy(gpuBuffers.current().getMappedMemory(), cpuBuffer.data(), cpuBuffer.alignedSize());
}

template<typename T>
constexpr VkDeviceSize DynamicSSBO<T>::getTotalRange() const {
    return cpuBuffer.alignedSize();
}

} // namespace buf
} // namespace rc
} // namespace bl

#endif

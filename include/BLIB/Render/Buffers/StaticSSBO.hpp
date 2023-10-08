#ifndef BLIB_RENDER_BUFFERS_STATICSSBO_HPP
#define BLIB_RENDER_BUFFERS_STATICSSBO_HPP

#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/AlignedBuffer.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Device-local SSBO intended for use with per-object data for static objects
 *
 * @tparam T The type of data to pack into the SSBO
 * @ingroup Renderer
 */
template<typename T>
class StaticSSBO : public tfr::Transferable {
public:
    /**
     * @brief Creates a new SSBO
     */
    StaticSSBO() = default;

    /**
     * @brief Creates a new SSBO
     *
     * @param vulkanState Renderer Vulkan state
     * @param size Number of elements to create
     */
    StaticSSBO(vk::VulkanState& vulkanState, std::uint32_t size);

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
    constexpr vk::Buffer& gpuBufferHandle();

    /**
     * @brief Queues the given range of elements to be copied to the GPU buffer
     *
     * @param start The first element to copy
     * @param numElements The number of elements to copy
     */
    void transferRange(std::uint32_t start, std::uint32_t numElements);

    /**
     * @brief Queues the transfer of all elements
     */
    void transferAll();

    /**
     * @brief Returns the size of the SSBO on the device
     */
    constexpr VkDeviceSize getTotalRange() const;

private:
    vk::AlignedBuffer<T> cpuBuffer;
    vk::Buffer gpuBuffer;
    std::uint32_t copyStart;
    std::uint32_t copyCount;

    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& context) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
StaticSSBO<T>::StaticSSBO(vk::VulkanState& vulkanState, std::uint32_t size) {
    create(vulkanState, size);
}

template<typename T>
void StaticSSBO<T>::create(vk::VulkanState& vs, std::uint32_t size) {
    vulkanState = &vs;
    copyStart   = 0;
    copyCount   = size;

    cpuBuffer.create(vs, vk::AlignedBuffer<T>::StorageBuffer, size);
    gpuBuffer.create(vs,
                     cpuBuffer.alignedSize(),
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     0);
}

template<typename T>
void StaticSSBO<T>::fill(const T& value) {
    cpuBuffer.fill(value);
}

template<typename T>
T& StaticSSBO<T>::operator[](std::uint32_t i) {
    return cpuBuffer[i];
}

template<typename T>
const T& StaticSSBO<T>::operator[](std::uint32_t i) const {
    return cpuBuffer[i];
}

template<typename T>
constexpr std::uint32_t StaticSSBO<T>::size() const {
    return cpuBuffer.size();
}

template<typename T>
inline void StaticSSBO<T>::destroy() {
    cpuBuffer.clear();
    gpuBuffer.destroy();
}

template<typename T>
inline constexpr vk::Buffer& StaticSSBO<T>::gpuBufferHandle() {
    return gpuBuffer;
}

template<typename T>
void StaticSSBO<T>::executeTransfer(VkCommandBuffer commandBuffer, tfr::TransferContext& context) {
    const VkDeviceSize copySize = copyCount * cpuBuffer.elementSize();

    VkBuffer stagingBuffer;
    void* dest;
    context.createTemporaryStagingBuffer(copySize, stagingBuffer, &dest);
    std::memcpy(dest, &cpuBuffer[copyStart], copySize);

    VkBufferCopy copyCmd{};
    copyCmd.dstOffset = copyStart * cpuBuffer.elementSize();
    copyCmd.size      = copySize;
    copyCmd.srcOffset = 0;
    vkCmdCopyBuffer(commandBuffer, stagingBuffer, gpuBuffer.getBuffer(), 1, &copyCmd);

    VkBufferMemoryBarrier bufBarrier{};
    bufBarrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    bufBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    bufBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    bufBarrier.buffer        = gpuBuffer.getBuffer();
    bufBarrier.offset        = copyCmd.dstOffset;
    bufBarrier.size          = copyCmd.size;
    context.registerBufferBarrier(bufBarrier);
}

template<typename T>
bool StaticSSBO<T>::ensureSize(std::uint32_t desiredSize) {
    if (desiredSize > cpuBuffer.size()) {
        std::uint32_t newSize = cpuBuffer.size() * 2;
        while (newSize < desiredSize) { newSize *= 2; }

        cpuBuffer.resize(newSize);
        gpuBuffer.ensureSize(cpuBuffer.alignedSize());

        return true;
    }
    return false;
}

template<typename T>
void StaticSSBO<T>::transferRange(std::uint32_t start, std::uint32_t numElements) {
    copyStart = start;
    copyCount = numElements;
    queueTransfer(SyncRequirement::Immediate);
}

template<typename T>
void StaticSSBO<T>::transferAll() {
    transferRange(0, cpuBuffer.size());
}

template<typename T>
constexpr VkDeviceSize StaticSSBO<T>::getTotalRange() const {
    return gpuBuffer.getSize();
}

} // namespace buf
} // namespace rc
} // namespace bl

#endif

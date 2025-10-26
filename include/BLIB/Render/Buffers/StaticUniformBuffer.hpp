#ifndef BLIB_RENDER_BUFFERS_STATICUNIFORMBUFFER_HPP
#define BLIB_RENDER_BUFFERS_STATICUNIFORMBUFFER_HPP

#include <BLIB/Render/Buffers/AlignedBuffer.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
/// Collection of common buffer classes
namespace buf
{
/**
 * @brief Device-local, staged, uniform buffer array. Intended for small, static data such as
 *        scene lighting data
 *
 * @tparam T The type of data to pack into the UBO
 * @ingroup Renderer
 */
template<typename T>
class StaticUniformBuffer : public tfr::Transferable {
public:
    /**
     * @brief Creates a new StaticUniformBuffer
     */
    StaticUniformBuffer() = default;

    /**
     * @brief Creates a new StaticUniformBuffer
     *
     * @param vulkanState Renderer Vulkan state
     * @param size Number of elements to create
     * @param alignment The alignment rule to follow when aligning array elements
     */
    StaticUniformBuffer(vk::VulkanState& vulkanState, std::uint32_t size,
                        Alignment alignment = Alignment::Std140);

    /**
     * @brief Creates a new StaticUniformBuffer
     *
     * @param vulkanState Renderer Vulkan state
     * @param size Number of elements to create
     * @param alignment The alignment rule to follow when aligning array elements
     */
    void create(vk::VulkanState& vulkanState, std::uint32_t size,
                Alignment alignment = Alignment::Std140);

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
     * @brief Returns the number of elements in the buffer
     */
    std::uint32_t size() const;

    /**
     * @brief Frees the GPU buffers
     */
    void destroy();

    /**
     * @brief Returns the Buffer objects of the GPU buffers
     */
    vk::Buffer& gpuBufferHandle();

    /**
     * @brief Returns the Buffer objects of the GPU buffers
     */
    const vk::Buffer& gpuBufferHandle() const;

    /**
     * @brief Returns the aligned size of contained uniforms
     */
    std::uint32_t alignedUniformSize() const;

    /**
     * @brief Returns the aligned size of all contained uniforms
     */
    std::uint32_t getTotalAlignedSize() const;

    /**
     * @brief Helper method to return the raw buffer handle for the given frame index
     *
     * @param frameIndex The frame index to get the buffer handle for
     * @return The raw buffer handle
     */
    VkBuffer getRawBuffer(std::uint32_t frameIndex) const;

private:
    buf::AlignedBuffer<T> cpuBuffer;
    vk::Buffer gpuBuffer;

    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& context) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
StaticUniformBuffer<T>::StaticUniformBuffer(vk::VulkanState& vulkanState, std::uint32_t size,
                                            Alignment alignment) {
    create(vulkanState, size, alignment);
}

template<typename T>
void StaticUniformBuffer<T>::create(vk::VulkanState& vs, std::uint32_t size, Alignment alignment) {
    vulkanState = &vs;
    cpuBuffer.create(alignment, size);

    gpuBuffer.create(vs,
                     cpuBuffer.alignedSize(),
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                     0);
}

template<typename T>
void StaticUniformBuffer<T>::fill(const T& value) {
    cpuBuffer.fill(value);
}

template<typename T>
T& StaticUniformBuffer<T>::operator[](std::uint32_t i) {
    return cpuBuffer[i];
}

template<typename T>
const T& StaticUniformBuffer<T>::operator[](std::uint32_t i) const {
    return cpuBuffer[i];
}

template<typename T>
std::uint32_t StaticUniformBuffer<T>::size() const {
    return cpuBuffer.size();
}

template<typename T>
void StaticUniformBuffer<T>::destroy() {
    cpuBuffer.clear();
    gpuBuffer.deferDestruction();
}

template<typename T>
vk::Buffer& StaticUniformBuffer<T>::gpuBufferHandle() {
    return gpuBuffer;
}

template<typename T>
const vk::Buffer& StaticUniformBuffer<T>::gpuBufferHandle() const {
    return gpuBuffer;
}

template<typename T>
std::uint32_t StaticUniformBuffer<T>::alignedUniformSize() const {
    return cpuBuffer.elementSize();
}

template<typename T>
std::uint32_t StaticUniformBuffer<T>::getTotalAlignedSize() const {
    return cpuBuffer.alignedSize();
}

template<typename T>
VkBuffer StaticUniformBuffer<T>::getRawBuffer(std::uint32_t) const {
    return gpuBuffer.getBuffer();
}

template<typename T>
void StaticUniformBuffer<T>::executeTransfer(VkCommandBuffer commandBuffer,
                                             tfr::TransferContext& context) {
    const VkDeviceSize len = cpuBuffer.alignedSize();

    VkBuffer stagingBuffer;
    void* stagingArea;
    context.createTemporaryStagingBuffer(len, stagingBuffer, &stagingArea);

    std::memcpy(stagingArea, cpuBuffer.data(), len);

    VkBufferCopy copyCmd{};
    copyCmd.dstOffset = 0;
    copyCmd.size      = len;
    copyCmd.srcOffset = 0;
    vkCmdCopyBuffer(commandBuffer, stagingBuffer, gpuBuffer.getBuffer(), 1, &copyCmd);

    VkBufferMemoryBarrier bufBarrier{};
    bufBarrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    bufBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    bufBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    bufBarrier.buffer        = gpuBuffer.getBuffer();
    bufBarrier.offset        = 0;
    bufBarrier.size          = len;
    context.registerBufferBarrier(bufBarrier);
}

} // namespace buf
} // namespace rc
} // namespace bl

#endif

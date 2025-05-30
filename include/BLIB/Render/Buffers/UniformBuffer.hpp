#ifndef BLIB_RENDER_BUFFERS_UNIFORMBUFFER_HPP
#define BLIB_RENDER_BUFFERS_UNIFORMBUFFER_HPP

#include <BLIB/Render/Buffers/Alignment.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
/// Collection of common buffer classes
namespace buf
{
/**
 * @brief Device-local, staged, uniform buffer array. Intended for small, dynamic data such as
 *        per-frame scene data. Not intended for larger data sets such as per-object data. Writes go
 *        directly to the staging buffer, so the used portions of the UBO must be written to every
 *        frame otherwise stale data will be copied to the GPU
 *
 * @tparam T The type of data to pack into the UBO
 * @ingroup Renderer
 */
template<typename T>
class UniformBuffer : public tfr::Transferable {
public:
    /**
     * @brief Creates a new UniformBuffer
     */
    UniformBuffer() = default;

    /**
     * @brief Creates a new UniformBuffer
     *
     * @param vulkanState Renderer Vulkan state
     * @param size Number of elements to create
     */
    UniformBuffer(vk::VulkanState& vulkanState, std::uint32_t size);

    /**
     * @brief Creates a new UniformBuffer
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
    vk::PerFrame<vk::Buffer>& gpuBufferHandles();

    /**
     * @brief Returns the aligned size of contained uniforms
     */
    std::uint32_t alignedUniformSize() const;

    /**
     * @brief Returns the size of all aligned values
     */
    std::uint32_t totalAlignedSize() const;

private:
    std::uint32_t count;
    std::uint32_t alignment;
    vk::PerFrame<vk::Buffer> gpuBuffers;
    vk::PerFrame<vk::Buffer> stagingBuffers;

    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& context) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
UniformBuffer<T>::UniformBuffer(vk::VulkanState& vulkanState, std::uint32_t size) {
    create(vulkanState, size);
}

template<typename T>
void UniformBuffer<T>::create(vk::VulkanState& vs, std::uint32_t size) {
    vulkanState = &vs;
    count       = size;
    alignment   = computeAlignment(sizeof(T), Alignment::UboBindOffset);

    gpuBuffers.init(vs, [this, &vs, size](vk::Buffer& buffer) {
        buffer.create(vs,
                      size * alignment,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                      0);
    });
    stagingBuffers.init(vs, [this, &vs, size](vk::Buffer& buffer) {
        buffer.create(vs,
                      size * alignment,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                          VMA_ALLOCATION_CREATE_MAPPED_BIT);
    });
}

template<typename T>
void UniformBuffer<T>::fill(const T& value) {
    for (std::uint32_t i = 0; i < count; ++i) { (*this)[i] = value; }
}

template<typename T>
T& UniformBuffer<T>::operator[](std::uint32_t i) {
    return *static_cast<T*>(static_cast<void*>(
        static_cast<std::uint8_t*>(stagingBuffers.current().getMappedMemory()) + i * alignment));
}

template<typename T>
const T& UniformBuffer<T>::operator[](std::uint32_t i) const {
    return *static_cast<T*>(static_cast<void*>(
        static_cast<std::uint8_t*>(stagingBuffers.current().getMappedMemory()) + i * alignment));
}

template<typename T>
std::uint32_t UniformBuffer<T>::size() const {
    return count;
}

template<typename T>
void UniformBuffer<T>::destroy() {
    gpuBuffers.cleanup([](vk::Buffer& buffer) { buffer.deferDestruction(); });
    stagingBuffers.cleanup([](vk::Buffer& buffer) { buffer.deferDestruction(); });
}

template<typename T>
vk::PerFrame<vk::Buffer>& UniformBuffer<T>::gpuBufferHandles() {
    return gpuBuffers;
}

template<typename T>
std::uint32_t UniformBuffer<T>::alignedUniformSize() const {
    return alignment;
}

template<typename T>
std::uint32_t UniformBuffer<T>::alignedUniformSize() const {
    return alignment * count;
}

template<typename T>
void UniformBuffer<T>::executeTransfer(VkCommandBuffer commandBuffer,
                                       tfr::TransferContext& context) {
    VkBufferCopy copyCmd{};
    copyCmd.dstOffset = 0;
    copyCmd.size      = stagingBuffers.current().getSize();
    copyCmd.srcOffset = 0;
    vkCmdCopyBuffer(commandBuffer,
                    stagingBuffers.current().getBuffer(),
                    gpuBuffers.current().getBuffer(),
                    1,
                    &copyCmd);

    VkBufferMemoryBarrier bufBarrier{};
    bufBarrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    bufBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    bufBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    bufBarrier.buffer        = gpuBuffers.current().getBuffer();
    bufBarrier.offset        = 0;
    bufBarrier.size          = copyCmd.size;
    context.registerBufferBarrier(bufBarrier);
}

} // namespace buf
} // namespace rc
} // namespace bl

#endif

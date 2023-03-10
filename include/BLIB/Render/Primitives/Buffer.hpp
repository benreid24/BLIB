#ifndef BLIB_RENDER_PRIMITIVES_BUFFER_HPP
#define BLIB_RENDER_PRIMITIVES_BUFFER_HPP

#include <BLIB/Render/Transfers/TransferEngine.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glad/vulkan.h>
#include <initializer_list>
#include <vector>

namespace bl
{
namespace render
{
/**
 * @brief Raw buffer that exists on CPU and in device-local GPU memory. Intended for data that does
 *        not update regularly as this uses staging buffers for transfers
 *
 * @tparam T The type to have a buffer of
 * @ingroup Renderer
 */
template<typename T>
class Buffer : public tfr::Transferable {
public:
    /**
     * @brief Creates an empty buffer
     */
    Buffer();

    /**
     * @brief Destroys the buffer and frees resources
     */
    virtual ~Buffer();

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
     * @brief Returns the number of elements in the buffer
     */
    constexpr std::size_t size() const;

    /**
     * @brief Returns a pointer to the start of the CPU data
     */
    constexpr T* data();

    /**
     * @brief Returns a pointer to the start of the CPU data
     */
    constexpr const T* data() const;

    /**
     * @brief Access a specific element in the CPU buffer
     *
     * @param i The index of the element to access
     * @return A reference to the element at the given index
     */
    constexpr T& operator[](std::size_t i);

    /**
     * @brief Access a specific element in the CPU buffer
     *
     * @param i The index of the element to access
     * @return A reference to the element at the given index
     */
    constexpr const T& operator[](std::size_t i) const;

    /**
     * @brief Convenience method to assign a set of values all at once
     *
     * @tparam U The type of value to use for assignment
     * @param values The values to assign to
     * @param start The index of the first buffer value to write to
     */
    template<typename U>
    void assign(std::initializer_list<U> values, std::size_t start = 0);

    /**
     * @brief Returns the Vulkan handle to the GPU buffer
     */
    constexpr VkBuffer handle() const;

private:
    std::vector<T> cpuBuffer;
    VkBuffer gpuBuffer;
    VkDeviceMemory gpuMemory;

    virtual void executeTransfer(VkCommandBuffer cb, tfr::TransferEngine& engine) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Buffer<T>::Buffer()
: Transferable() {}

template<typename T>
Buffer<T>::~Buffer() {
    destroy();
}

template<typename T>
void Buffer<T>::create(VulkanState& vs, std::uint32_t size, VkBufferUsageFlags usage) {
    destroy();
    vulkanState = &vs;

    cpuBuffer.resize(size);
    vs.createBuffer(sizeof(T) * size,
                    usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    gpuBuffer,
                    gpuMemory);
}

template<typename T>
void Buffer<T>::destroy() {
    if (vulkanState != nullptr) {
        vkDestroyBuffer(vulkanState->device, gpuBuffer, nullptr);
        vkFreeMemory(vulkanState->device, gpuMemory, nullptr);
        vulkanState = nullptr;
    }
    cpuBuffer.clear();
}

template<typename T>
constexpr std::size_t Buffer<T>::size() const {
    return cpuBuffer.size();
}

template<typename T>
constexpr T* Buffer<T>::data() {
    return cpuBuffer.data();
}

template<typename T>
constexpr const T* Buffer<T>::data() const {
    return cpuBuffer.data();
}

template<typename T>
constexpr T& Buffer<T>::operator[](std::size_t i) {
    return cpuBuffer[i];
}

template<typename T>
constexpr const T& Buffer<T>::operator[](std::size_t i) const {
    return cpuBuffer[i];
}

template<typename T>
template<typename U>
void Buffer<T>::assign(std::initializer_list<U> values, std::size_t start) {
    std::size_t i = start;
    for (const U& v : values) {
        cpuBuffer[i] = v;
        ++i;
    }
}

template<typename T>
void Buffer<T>::executeTransfer(VkCommandBuffer commandBuffer, tfr::TransferEngine& engine) {
    const VkDeviceSize size = sizeof(T) * cpuBuffer.size();

    // TODO - keep staging buffer around?
    // create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    engine.createStagingBuffer(size, stagingBuffer, stagingMemory);

    // map and copy to staging
    void* mappedAddress = nullptr;
    vkMapMemory(vulkanState->device, stagingMemory, 0, size, 0, &mappedAddress);
    memcpy(mappedAddress, cpuBuffer.data(), static_cast<std::size_t>(size));
    vkUnmapMemory(vulkanState->device, stagingMemory);

    // copy staging -> gpu
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size      = size;
    vkCmdCopyBuffer(commandBuffer, stagingBuffer, gpuBuffer, 1, &copyRegion);

    // insert pipeline barrier
    VkBufferMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
    barrier.buffer              = gpuBuffer;
    barrier.offset              = 0;
    barrier.size                = size;
    engine.registerBufferBarrier(barrier);
}

template<typename T>
constexpr VkBuffer Buffer<T>::handle() const {
    return gpuBuffer;
}

} // namespace render
} // namespace bl

#endif

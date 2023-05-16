#ifndef BLIB_RENDER_TRANSFERS_UNIFORMBUFFER_HPP
#define BLIB_RENDER_TRANSFERS_UNIFORMBUFFER_HPP

#include <BLIB/Render/Transfers/GenericBuffer.hpp>
#include <BLIB/Render/Vulkan/AlignedBuffer.hpp>

namespace bl
{
namespace render
{
namespace tfr
{
/**
 * @brief Specialized buffer intended to hold arrays of uniform objects. Provides dynamically
 *        aligned CPU storage and a double-buffered, staged, GPU storage
 *
 * @tparam T The type of uniform to store an array of
 * @ingroup Renderer
 */
template<typename T>
class UniformBuffer : public Transferable {
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
     * @brief Changes the size of the buffer
     *
     * @param size New number of elements to store
     */
    void resize(std::uint32_t size);

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
    constexpr std::uint32_t size() const;

    /**
     * @brief Configures which section of the buffer should transfer from CPU to GPU
     *
     * @param firstElement Index of the first element to transfer
     * @param elementCount Number of elements to transfer
     */
    void configureTransferRange(std::uint32_t firstElement, std::uint32_t elementCount);

    /**
     * @brief Configures to transfer all elements from CPU to GPU
     */
    void configureTransferAll();

    /**
     * @brief Frees the GPU buffers
     */
    void destroy();

    /**
     * @brief Returns the VkBuffer handles of the GPU buffers
     */
    constexpr vk::PerFrame<VkBuffer>& gpuBufferHandles();

    /**
     * @brief Returns the aligned size of contained uniforms
     */
    constexpr std::uint32_t alignedUniformSize() const;

private:
    vk::AlignedBuffer<T> cpuBuffer;
    GenericBuffer<VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true> gpuBuffer;

    virtual void executeTransfer(VkCommandBuffer commandBuffer, TransferContext& context) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
UniformBuffer<T>::UniformBuffer(vk::VulkanState& vulkanState, std::uint32_t size) {
    create(vulkanState, size);
}

template<typename T>
void UniformBuffer<T>::create(vk::VulkanState& vs, std::uint32_t size) {
    vulkanState = &vs;
    cpuBuffer.create(vs, size);
    gpuBuffer.create(vs, cpuBuffer.alignedSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    configureTransferAll();
}

template<typename T>
void UniformBuffer<T>::resize(std::uint32_t size) {
    cpuBuffer.resize(size);
    if (cpuBuffer.alignedSize() > gpuBuffer.size()) {
        gpuBuffer.create(*vulkanState, cpuBuffer.alignedSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }
}

template<typename T>
void UniformBuffer<T>::fill(const T& value) {
    cpuBuffer.fill(value);
}

template<typename T>
T& UniformBuffer<T>::operator[](std::uint32_t i) {
    return cpuBuffer[i];
}

template<typename T>
const T& UniformBuffer<T>::operator[](std::uint32_t i) const {
    return cpuBuffer[i];
}

template<typename T>
constexpr std::uint32_t UniformBuffer<T>::size() const {
    return cpuBuffer.size();
}

template<typename T>
void UniformBuffer<T>::configureTransferRange(std::uint32_t firstElement,
                                              std::uint32_t elementCount) {
    const std::uint32_t offset = firstElement * cpuBuffer.elementSize();
    gpuBuffer.configureWrite(static_cast<const char*>(cpuBuffer.data()) + offset,
                             offset,
                             elementCount * cpuBuffer.elementSize());
}

template<typename T>
void UniformBuffer<T>::configureTransferAll() {
    gpuBuffer.configureWrite(cpuBuffer.data(), 0, cpuBuffer.alignedSize());
}

template<typename T>
inline void UniformBuffer<T>::destroy() {
    gpuBuffer.destroy();
}

template<typename T>
inline constexpr vk::PerFrame<VkBuffer>& UniformBuffer<T>::gpuBufferHandles() {
    return gpuBuffer.handles();
}

template<typename T>
inline constexpr std::uint32_t UniformBuffer<T>::alignedUniformSize() const {
    return cpuBuffer.elementSize();
}

template<typename T>
void UniformBuffer<T>::executeTransfer(VkCommandBuffer commandBuffer, TransferContext& context) {
    gpuBuffer.executeTransfer(commandBuffer, context);
}

} // namespace tfr
} // namespace render
} // namespace bl

#endif

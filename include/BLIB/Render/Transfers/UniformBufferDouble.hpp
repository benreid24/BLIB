#ifndef BLIB_RENDER_TRANSFERS_UNIFORMBUFFERDOUBLE_HPP
#define BLIB_RENDER_TRANSFERS_UNIFORMBUFFERDOUBLE_HPP

#include <BLIB/Render/Transfers/GenericBuffer.hpp>
#include <BLIB/Render/Vulkan/AlignedBuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>

namespace bl
{
namespace gfx
{
namespace tfr
{
/**
 * @brief Specialized buffer intended to hold arrays of uniform objects. Provides dynamically
 *        aligned, double-buffered CPU storage and a double-buffered, staged, GPU storage
 *
 * @tparam T The type of uniform to store an array of
 * @ingroup Renderer
 */
template<typename T>
class UniformBufferDouble : public Transferable {
public:
    /**
     * @brief Creates a new UniformBufferDouble
     */
    UniformBufferDouble() = default;

    /**
     * @brief Creates a new UniformBufferDouble
     *
     * @param vulkanState Renderer Vulkan state
     * @param size Number of elements to create
     */
    UniformBufferDouble(vk::VulkanState& vulkanState, std::uint32_t size);

    /**
     * @brief Creates a new UniformBufferDouble
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
     * @brief Fills both buffers with the given value
     *
     * @param value The value to copy into all existing slots
     */
    void fillAll(const T& value);

    /**
     * @brief Fills the current buffer with the given value
     *
     * @param value The value to copy into all existing slots
     */
    void fillCurrent(const T& value);

    /**
     * @brief Returns the element at the given index in the current buffer
     *
     * @param i Index of the element to access. Not bounds checked
     * @return A reference to the element at the given index
     */
    T& operator[](std::uint32_t i);

    /**
     * @brief Returns the element at the given index in the current buffer
     *
     * @param i Index of the element to access. Not bounds checked
     * @return A reference to the element at the given index
     */
    const T& operator[](std::uint32_t i) const;

    /**
     * @brief Writes data into both buffers
     *
     * @param index The object index to write
     * @param data The per-frame data to write
     */
    void write(std::uint32_t index, const vk::PerFrame<T>& data);

    /**
     * @brief Writes the given value into both buffers at the given index
     *
     * @param index The index to write
     * @param data The value to write to both buffers
     */
    void write(std::uint32_t index, const T& data);

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
     * @brief Returns the raw pointers to use to write to the given index
     *
     * @param index The index to get pointers to
     * @return The per-frame pointers to write to
     */
    std::array<T*, Config::MaxConcurrentFrames> getWriteLocations(std::uint32_t index);

    /**
     * @brief Returns the VkBuffer handles of the GPU buffers
     */
    constexpr vk::PerFrame<VkBuffer>& gpuBufferHandles();

    /**
     * @brief Returns the aligned size of contained uniforms
     */
    constexpr std::uint32_t alignedUniformSize() const;

private:
    vk::PerFrame<vk::AlignedBuffer<T>> cpuBuffer; // TODO - update class and getters
    GenericBuffer<VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true> gpuBuffer;

    virtual void executeTransfer(VkCommandBuffer commandBuffer, TransferContext& context) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
UniformBufferDouble<T>::UniformBufferDouble(vk::VulkanState& vulkanState, std::uint32_t size) {
    create(vulkanState, size);
}

template<typename T>
void UniformBufferDouble<T>::create(vk::VulkanState& vs, std::uint32_t size) {
    vulkanState = &vs;
    cpuBuffer.init(vs, [&vs, size](vk::AlignedBuffer<T>& buf) { buf.create(vs, size); });
    gpuBuffer.create(vs, cpuBuffer.current().alignedSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    configureTransferAll();
}

template<typename T>
void UniformBufferDouble<T>::resize(std::uint32_t size) {
    cpuBuffer.cleanup([size](vk::AlignedBuffer<T>& buf) { buf.resize(size); });
    if (cpuBuffer.current().alignedSize() > gpuBuffer.size()) {
        gpuBuffer.create(
            *vulkanState, cpuBuffer.current().alignedSize(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }
}

template<typename T>
void UniformBufferDouble<T>::fillAll(const T& value) {
    cpuBuffer.cleanup([&value](vk::AlignedBuffer<T>& buf) { buf.fill(value); });
}

template<typename T>
void UniformBufferDouble<T>::fillCurrent(const T& value) {
    cpuBuffer.current().fill(value);
}

template<typename T>
T& UniformBufferDouble<T>::operator[](std::uint32_t i) {
    return cpuBuffer.current()[i];
}

template<typename T>
const T& UniformBufferDouble<T>::operator[](std::uint32_t i) const {
    return cpuBuffer.current()[i];
}

template<typename T>
constexpr std::uint32_t UniformBufferDouble<T>::size() const {
    return cpuBuffer.current().size();
}

template<typename T>
void UniformBufferDouble<T>::write(std::uint32_t i, const T& value) {
    cpuBuffer.cleanup([&value, i](vk::AlignedBuffer<T>& buf) { buf[i] = value; });
}

template<typename T>
void UniformBufferDouble<T>::write(std::uint32_t i, const vk::PerFrame<T>& value) {
    for (unsigned int j = 0; j < Config::MaxConcurrentFrames; ++j) {
        cpuBuffer.getRaw(j)[i] = value.getRaw(j);
    }
}

template<typename T>
void UniformBufferDouble<T>::configureTransferRange(std::uint32_t firstElement,
                                                    std::uint32_t elementCount) {
    const std::uint32_t offset = firstElement * cpuBuffer.current().elementSize();
    gpuBuffer.configureWrite(static_cast<const char*>(cpuBuffer.current().data()) + offset,
                             offset,
                             elementCount * cpuBuffer.current().elementSize());
}

template<typename T>
void UniformBufferDouble<T>::configureTransferAll() {
    gpuBuffer.configureWrite(cpuBuffer.current().data(), 0, cpuBuffer.current().alignedSize());
}

template<typename T>
inline void UniformBufferDouble<T>::destroy() {
    gpuBuffer.destroy();
}

template<typename T>
inline std::array<T*, Config::MaxConcurrentFrames> UniformBufferDouble<T>::getWriteLocations(
    std::uint32_t index) {
    std::array<T*, Config::MaxConcurrentFrames> result;
    for (unsigned int i = 0; i < Config::MaxConcurrentFrames; ++i) {
        result[i] = &cpuBuffer.getRaw(i)[index];
    }
    return result;
}

template<typename T>
inline constexpr vk::PerFrame<VkBuffer>& UniformBufferDouble<T>::gpuBufferHandles() {
    return gpuBuffer.handles();
}

template<typename T>
inline constexpr std::uint32_t UniformBufferDouble<T>::alignedUniformSize() const {
    return cpuBuffer.current().elementSize();
}

template<typename T>
void UniformBufferDouble<T>::executeTransfer(VkCommandBuffer commandBuffer,
                                             TransferContext& context) {
    gpuBuffer.executeTransfer(commandBuffer, context);
}

} // namespace tfr
} // namespace gfx
} // namespace bl

#endif

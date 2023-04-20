#ifndef BLIB_RENDER_PRIMITIVES_GENERICBUFFER_HPP
#define BLIB_RENDER_PRIMITIVES_GENERICBUFFER_HPP

#include <BLIB/Render/Transfers/Transferable.hpp>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
struct VulkanState;

/// Collection of renderer primitives
namespace prim
{
template<typename T, VkMemoryPropertyFlags Memory, bool DoubleBuffer>
class GenericBuffer : public tfr::Transferable {
    static constexpr bool StageRequired = Memory & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT != 0;

public:
    /**
     * @brief Creates an empty buffer
     */
    GenericBuffer();

    /**
     * @brief Destroys the buffer and frees resources
     */
    virtual ~GenericBuffer();

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
     * @brief Queues the given data to be written to the buffer. Pointer must stay valid
     *
     * @param data Pointer to the data to write
     * @param len Number of elements to write
     */
    void write(const T* data, std::size_t len);

    /**
     * @brief Returns the number of elements in the buffer
     */
    constexpr std::size_t size() const;

    /**
     * @brief Returns the Vulkan handle to the GPU buffer
     */
    constexpr VkBuffer handle() const;

private:
    //
};

} // namespace prim
} // namespace render
} // namespace bl

#endif

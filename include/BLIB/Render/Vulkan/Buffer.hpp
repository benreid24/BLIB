#ifndef BLIB_RENDER_BUFFERS_BUFFER_HPP
#define BLIB_RENDER_BUFFERS_BUFFER_HPP

#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Wrapper over Vulkan buffers. Provides simpler interfaces and resize ability
 *
 * @ingroup Renderer
 */
class Buffer {
public:
    /**
     * @brief Does nothing
     */
    Buffer();

    /**
     * @brief Destroys the buffer if created
     */
    ~Buffer();

    /**
     * @brief Creates the buffer with the given parameters
     *
     * @param vulkanState The renderer Vulkan state
     * @param size The size of the buffer in bytes
     * @param memPool Which memory type to use
     * @param usage How the buffer will be used
     * @param allocFlags VMA allocation flags to use for the VMA allocation
     * @return True if the buffer was able to be created, false otherwise
     */
    bool create(VulkanState& vulkanState, VkDeviceSize size, VkMemoryPropertyFlags memPool,
                VkBufferUsageFlags usage, VmaAllocationCreateFlags allocFlags);

    /**
     * @brief Creates the buffer with the given parameters and a backup memory type
     *
     * @param vulkanState The renderer Vulkan state
     * @param size The size of the buffer in bytes
     * @param memPool Which memory type to use
     * @param fallbackPool Memory type to use if the desired memory type fails creation
     * @param usage How the buffer will be used
     * @param allocFlags VMA allocation flags to use for the VMA allocation
     * @return True if the buffer was able to be created, false otherwise
     */
    bool createWithFallback(VulkanState& vulkanState, VkDeviceSize size,
                            VkMemoryPropertyFlags memPool, VkMemoryPropertyFlags fallbackPool,
                            VkBufferUsageFlags usage, VmaAllocationCreateFlags allocFlags);

    /**
     * @brief If the buffer size is less than the required size, re-creates the buffer to be at
     *        least the given size
     *
     * @param size The size the buffer should be, at a minimum
     * @return True if the buffer was resized, false if it was big enough
     */
    bool ensureSize(VkDeviceSize size);

    /**
     * @brief Returns whether or not the buffer is currently created
     */
    constexpr bool created() const;

    /**
     * @brief Destroys the buffer immediately
     */
    void destroy();

    /**
     * @brief Defers destruction of the buffer for Config::MaxConcurrentFrames + 1 frames. Buffer
     *        can be re-used immediately
     */
    void deferDestruction();

    /**
     * @brief Returns the Vulkan buffer handle
     */
    constexpr VkBuffer getBuffer() const;

    /**
     * @brief Returns the VMA allocation
     */
    constexpr VmaAllocation getAlloc() const;

    /**
     * @brief Maps the buffer. Does not check if buffer is map-able or if it is already mapped
     * @return A pointer to the mapped memory
     */
    void* mapMemory();

    /**
     * @brief Returns a pointer to the mapped buffer memory
     */
    constexpr void* getMappedMemory() const;

    /**
     * @brief Unmaps the buffer. Does not check if the buffer is currently mapped
     */
    void unMapMemory();

    /**
     * @brief Returns the size of the buffer in bytes
     */
    constexpr VkDeviceSize getSize() const;

private:
    VulkanState* vulkanState;
    VkBuffer buffer;
    VmaAllocation alloc;
    void* mapped;
    VmaAllocationCreateFlags allocFlags;
    VkDeviceSize size;
    VkMemoryPropertyFlags memPool;
    VkBufferUsageFlags usage;

    bool doCreate();
    void doDefer();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr bool Buffer::created() const { return vulkanState != nullptr; }

inline constexpr VkBuffer Buffer::getBuffer() const { return buffer; }

inline constexpr VmaAllocation Buffer::getAlloc() const { return alloc; }

inline constexpr void* Buffer::getMappedMemory() const { return mapped; }

inline constexpr VkDeviceSize Buffer::getSize() const { return size; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif

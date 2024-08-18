#ifndef BLIB_RENDER_VULKAN_SHAREDCOMMANDBUFFER_HPP
#define BLIB_RENDER_VULKAN_SHAREDCOMMANDBUFFER_HPP

#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
class SharedCommandPool;

/**
 * @brief Wrapper around a command buffer allocated from a SharedCommandPool
 *
 * @ingroup Renderer
 */
class SharedCommandBuffer {
public:
    /**
     * @brief Move constructor
     *
     * @param move The command buffer to move from
     */
    SharedCommandBuffer(SharedCommandBuffer&& move);

    /**
     * @brief Releases the lock on the pool if not already submitted
     */
    ~SharedCommandBuffer();

    /**
     * @brief Access the underlying command buffer
     */
    operator VkCommandBuffer() { return buffer; }

    /**
     * @brief Access the underlying command buffer
     */
    VkCommandBuffer get() { return buffer; }

    /**
     * @brief Submits the buffer and releases the lock on the pool
     */
    void submit();

private:
    SharedCommandPool* owner;
    VkFence fence;
    VkCommandBuffer buffer;

    SharedCommandBuffer(SharedCommandPool* owner, VkFence fence, VkCommandBuffer buffer);

    SharedCommandBuffer()                                      = delete;
    SharedCommandBuffer(const SharedCommandBuffer&)            = delete;
    SharedCommandBuffer& operator=(const SharedCommandBuffer&) = delete;
    SharedCommandBuffer& operator=(SharedCommandBuffer&& move) = delete;

    friend class SharedCommandPool;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

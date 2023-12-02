#ifndef BLIB_RENDER_VULKAN_SHAREDCOMMANDPOOL_HPP
#define BLIB_RENDER_VULKAN_SHAREDCOMMANDPOOL_HPP

#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/SharedCommandBuffer.hpp>
#include <BLIB/Vulkan.hpp>
#include <mutex>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Wrapper around a Vulkan command pool that is intended to be shared for short-lived command
 *        buffers. Provides thread safety and automatic command buffer cleanup
 *
 * @ingroup Renderer
 */
class SharedCommandPool {
public:
    /**
     * @brief Does nothing
     */
    SharedCommandPool();

    /**
     * @brief Creates the shared command pool
     *
     * @param vulkanState The renderer Vulkan state
     */
    void create(VulkanState& vulkanState);

    /**
     * @brief Called once at the beginning of each frame
     */
    void onFrameStart();

    /**
     * @brief Frees the shared command pool
     */
    void cleanup();

    /**
     * @brief Allocates a buffer from the shared pool
     *
     * @param usage How the command buffer will be used
     * @return The new command buffer
     */
    SharedCommandBuffer createBuffer(VkCommandBufferUsageFlags usage = 0);

private:
    std::mutex mutex;
    VulkanState* vs;
    PerFrame<VkCommandPool> pool;

    void submit(SharedCommandBuffer& buffer);

    friend class SharedCommandBuffer;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

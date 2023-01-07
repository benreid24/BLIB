#ifndef BLIB_RENDER_VULKAN_SWAPFRAME_HPP
#define BLIB_RENDER_VULKAN_SWAPFRAME_HPP

#include <glad/vulkan.h>
#include <stdexcept>

namespace bl
{
namespace render
{
struct VulkanState;

/**
 * @brief Utility struct that represents a swap frame in the window's swap chain. Manages
 *        synchronization primitives and resource cleanup. Also owns per-frame resources such as
 *        command pools and buffers to aid in thread safety for multi-threaded rendering
 *
 * @ingroup Renderer
 */
struct SwapFrame {
    VkDevice deviceInitedWith;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    /// @brief Creates an uninitialized swap frame
    SwapFrame() = default;

    /**
     * @brief Destroy the Swap Frame object
     *
     */
    ~SwapFrame();

    /**
     * @brief Construct a new Swap Frame
     *
     * @param vulkanState The engine Vulkan state
     */
    void initialize(VulkanState& vulkanState);

    /**
     * @brief Frees owned resources
     *
     */
    void cleanup();
};

} // namespace render
} // namespace bl

#endif

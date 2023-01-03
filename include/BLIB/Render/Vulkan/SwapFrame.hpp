#ifndef BLIB_RENDER_VULKAN_SWAPFRAME_HPP
#define BLIB_RENDER_VULKAN_SWAPFRAME_HPP

#include <glad/vulkan.h>
#include <stdexcept>

namespace bl
{
namespace render
{
/**
 * @brief Utility struct that represents a swap frame in the window's swap chain. Manages
 *        synchronization primitives and resource cleanup
 *
 * @ingroup Renderer
 */
struct SwapFrame {
    VkDevice deviceInitedWith;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    /// @brief Creates an uninitialized swap frame
    SwapFrame() = default;

    /**
     * @brief Construct a new Swap Frame 
     * 
     * @param device The Vulkan device to use
     * @param commandPool The command pool to create the primary command buffer with
     */
    SwapFrame(VkDevice device, VkCommandPool commandPool);

    /**
     * @brief Destroy the Swap Frame object
     * 
     */
    ~SwapFrame();

    /**
     * @brief Construct a new Swap Frame 
     * 
     * @param device The Vulkan device to use
     * @param commandPool The command pool to create the primary command buffer with
     */
    void initialize(VkDevice device, VkCommandPool commandPool);

    /**
     * @brief Frees owned resources
     * 
     */
    void cleanup();
};

} // namespace render
} // namespace bl

#endif

#ifndef BLIB_RENDER_VULKAN_DEDICATEDCOMMANDBUFFERS_HPP
#define BLIB_RENDER_VULKAN_DEDICATEDCOMMANDBUFFERS_HPP

#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Helper class that provides per-frame command buffers intended for reuse
 */
class DedicatedCommandBuffers : private util::NonCopyable {
public:
    /**
     * @brief Does nothing
     */
    DedicatedCommandBuffers();

    /**
     * @brief Releases the command pools
     */
    ~DedicatedCommandBuffers();

    /**
     * @brief Creates the command pools and buffers
     *
     * @param vulkanState The renderer Vulkan state
     */
    void create(VulkanState& vulkanState);

    /**
     * @brief Releases the command pools
     */
    void destroy();

    /**
     * @brief Waits for the command buffer to enter a valid state, begins recording, and returns it
     */
    VkCommandBuffer begin();

    /**
     * @brief Submits the current command buffer
     */
    void submit();

private:
    struct Payload {
        VkCommandPool pool;
        VkCommandBuffer buffer;
        VkFence fence;

        void create(VulkanState& vs);
        void cleanup(VulkanState& vs);
    };

    VulkanState* vs;
    PerFrame<Payload> payloads;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

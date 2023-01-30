#ifndef BLIB_RENDER_VULKAN_TRANSFERABLE_HPP
#define BLIB_RENDER_VULKAN_TRANSFERABLE_HPP

#include <glad/vulkan.h>

namespace bl
{
namespace render
{
struct VulkanState;
class TransferEngine;

/**
 * @brief Base class for objects that transfer state or data to the GPU. Used in conjuction with the
 *        TransferEngine to execute and synchornous resource transfers and subsequent access
 *
 * @ingroup Renderer
 */
class Transferable {
public:
    /**
     * @brief Destroy the transferable object
     */
    virtual ~Transferable() = default;

    /**
     * @brief Queues this object with the TransferEngine to be transfered on the next frame
     */
    void queueTransfer();

protected:
    VulkanState* vulkanState;

    /**
     * @brief Sets vulkanState to nullptr
     */
    Transferable();

    /**
     * @brief Initializes the reference to the VulkanState
     *
     * @param vs Renderer VulkanState
     */
    Transferable(VulkanState& vs);

    /**
     * @brief Derived classes should perform their transfers here. Barriers should be registered
     *        with transferEngine. Barriers are recorded after all transfer commands are recorded
     *
     * @param commandBuffer Command buffer to use to perform the transfer
     * @param transferEngine Transfer engine that is copying this object
     */
    virtual void executeTransfer(VkCommandBuffer commandBuffer, TransferEngine& transferEngine) = 0;

private:
    friend class TransferEngine;
};

} // namespace render
} // namespace bl

#endif

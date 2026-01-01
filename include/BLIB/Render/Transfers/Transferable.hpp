#ifndef BLIB_RENDER_VULKAN_TRANSFERABLE_HPP
#define BLIB_RENDER_VULKAN_TRANSFERABLE_HPP

#include <BLIB/Render/Transfers/TransferContext.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanLayer;
}

/// Module wrapping the TransferEngine and related mechanisms for moving data to the GPU
namespace tfr
{
/**
 * @brief Base class for objects that transfer state or data to the GPU. Used in conjunction with
 *        the TransferEngine to execute and synchronous resource transfers and subsequent access
 *
 * @ingroup Renderer
 */
class Transferable {
public:
    /// Enum containing requirements for when transfers may start
    enum struct SyncRequirement {
        /// Transfers with this requirement may be started immediately
        Immediate,

        /// Transfers with this requirement cannot start until the device is idle
        DeviceIdle
    };

    /**
     * @brief Destroy the transferable object
     */
    virtual ~Transferable();

    /**
     * @brief Queues this object with the TransferEngine to be transfered on the next frame
     *
     * @param syncReq Synchronization requirements for the transfer
     * @return True if the transfer was scheduled, false if already scheduled
     */
    bool queueTransfer(SyncRequirement syncReq = SyncRequirement::Immediate);

    /**
     * @brief Configures this Transferable to transfer every frame with the given sync requirement
     *
     * @param syncReq Synchronization requirements for the transfer
     */
    void transferEveryFrame(SyncRequirement syncReq = SyncRequirement::Immediate);

    /**
     * @brief Removes this Transferable from the queue that gets transferred every frame
     */
    void stopTransferringEveryFrame();

    /**
     * @brief Returns whether or not a transfer is queued, either individually or every frame
     */
    bool isQueued() const { return queued || perFrame != NotPerFrame; }

    /**
     * @brief Cancels the scheduled transfer. Also cancels transfers that occur every frame
     */
    void cancelQueuedTransfer();

protected:
    vk::VulkanLayer* vulkanState;

    /**
     * @brief Sets vulkanState to nullptr
     */
    Transferable();

    /**
     * @brief Initializes the reference to the VulkanLayer
     *
     * @param vs Renderer VulkanLayer
     */
    Transferable(vk::VulkanLayer& vs);

    /**
     * @brief Derived classes should perform their transfers here. Barriers should be registered
     *        with transferEngine. Barriers are recorded after all transfer commands are recorded
     *
     * @param commandBuffer Command buffer to use to perform the transfer
     * @param context TransferContext to use to perform transfer operations
     */
    virtual void executeTransfer(VkCommandBuffer commandBuffer, TransferContext& context) = 0;

private:
    static constexpr SyncRequirement NotPerFrame =
        static_cast<SyncRequirement>(static_cast<int>(SyncRequirement::Immediate) +
                                     static_cast<int>(SyncRequirement::DeviceIdle) + 1);

    SyncRequirement perFrame;
    bool queued;

    friend class TransferEngine;
};

} // namespace tfr
} // namespace rc
} // namespace bl

#endif

#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

#include <unordered_set>

namespace bl
{
namespace rc
{
namespace tfr
{
Transferable::Transferable()
: vulkanState(nullptr)
, perFrame(NotPerFrame)
, queued(false) {}

Transferable::Transferable(vk::VulkanLayer& vs)
: vulkanState(&vs)
, perFrame(NotPerFrame)
, queued(false) {}

Transferable::~Transferable() { cancelQueuedTransfer(); }

void Transferable::cancelQueuedTransfer() {
    if (perFrame != NotPerFrame) { stopTransferringEveryFrame(); }
    else if (queued) { vulkanState->getTransferEngine().cancelTransfer(this); }
}

bool Transferable::queueTransfer(SyncRequirement syncReq) {
#ifdef BLIB_DEBUG
    if (vulkanState == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Vulkan state";
        return false;
    }
#endif

    if (!queued) {
        queued = true;
        vulkanState->getTransferEngine().queueOneTimeTransfer(this, syncReq);
        return true;
    }
    return false;
}

void Transferable::transferEveryFrame(SyncRequirement syncReq) {
#ifdef BLIB_DEBUG
    if (vulkanState == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Vulkan state";
    }
#endif

    queued   = true;
    perFrame = syncReq;
    vulkanState->getTransferEngine().registerPerFrameTransfer(this, syncReq);
}

void Transferable::stopTransferringEveryFrame() {
#ifdef BLIB_DEBUG
    if (vulkanState == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Vulkan state";
    }
#endif

    vulkanState->getTransferEngine().unregisterPerFrameTransfer(this, perFrame);
    perFrame = NotPerFrame;
    queued   = false;
}

} // namespace tfr
} // namespace rc
} // namespace bl

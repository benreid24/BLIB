#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
namespace tfr
{
Transferable::Transferable()
: vulkanState(nullptr)
, perFrame(NotPerFrame) {}

Transferable::Transferable(VulkanState& vs)
: vulkanState(&vs)
, perFrame(NotPerFrame) {}

Transferable::~Transferable() {
    if (perFrame != NotPerFrame) { stopTransferringEveryFrame(); }
}

void Transferable::queueTransfer(SyncRequirement syncReq) {
#ifdef BLIB_DEBUG
    if (vulkanState == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Vulkan state";
    }
#endif

    vulkanState->transferEngine.queueOneTimeTransfer(this, syncReq);
}

void Transferable::transferEveryFrame(SyncRequirement syncReq) {
#ifdef BLIB_DEBUG
    if (vulkanState == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Vulkan state";
    }
#endif

    perFrame = syncReq;
    vulkanState->transferEngine.registerPerFrameTransfer(this, syncReq);
}

void Transferable::stopTransferringEveryFrame() {
#ifdef BLIB_DEBUG
    if (vulkanState == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Vulkan state";
    }
#endif

    vulkanState->transferEngine.unregisterPerFrameTransfer(this, perFrame);
    perFrame = NotPerFrame;
}

} // namespace tfr
} // namespace render
} // namespace bl

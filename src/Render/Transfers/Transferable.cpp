#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

#include <unordered_set>

namespace bl
{
namespace rc
{
namespace tfr
{
namespace
{
std::unordered_set<Transferable*> destroyed;
}

Transferable::Transferable()
: vulkanState(nullptr)
, perFrame(NotPerFrame)
, queued(false) {}

Transferable::Transferable(vk::VulkanState& vs)
: vulkanState(&vs)
, perFrame(NotPerFrame)
, queued(false) {}

Transferable::~Transferable() {
    if (perFrame != NotPerFrame) { stopTransferringEveryFrame(); }
    else if (queued) { vulkanState->transferEngine.cancelTransfer(this); }
    destroyed.insert(this);
}

void Transferable::queueTransfer(SyncRequirement syncReq) {
#ifdef BLIB_DEBUG
    if (vulkanState == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Vulkan state";
    }
#endif

    if (destroyed.find(this) != destroyed.end()) {
        BL_LOG_ERROR << "wtf";
        //
    }

    if (!queued) {
        queued = true;
        vulkanState->transferEngine.queueOneTimeTransfer(this, syncReq);
    }
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
} // namespace rc
} // namespace bl

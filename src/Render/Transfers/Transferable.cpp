#include <BLIB/Render/Transfers/Transferable.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <unordered_set>

namespace bl
{
namespace rc
{
namespace tfr
{
Transferable::Transferable()
: renderer(nullptr)
, perFrame(NotPerFrame)
, queued(false) {}

Transferable::Transferable(Renderer& renderer)
: renderer(&renderer)
, perFrame(NotPerFrame)
, queued(false) {}

Transferable::~Transferable() { cancelQueuedTransfer(); }

void Transferable::cancelQueuedTransfer() {
    if (perFrame != NotPerFrame) { stopTransferringEveryFrame(); }
    else if (queued) { renderer->getTransferEngine().cancelTransfer(this); }
}

bool Transferable::queueTransfer(SyncRequirement syncReq) {
#ifdef BLIB_DEBUG
    if (renderer == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Renderer";
        return false;
    }
#endif

    if (!queued) {
        queued = true;
        renderer->getTransferEngine().queueOneTimeTransfer(this, syncReq);
        return true;
    }
    return false;
}

void Transferable::transferEveryFrame(SyncRequirement syncReq) {
#ifdef BLIB_DEBUG
    if (renderer == nullptr) { BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Renderer"; }
#endif

    queued   = true;
    perFrame = syncReq;
    renderer->getTransferEngine().registerPerFrameTransfer(this, syncReq);
}

void Transferable::stopTransferringEveryFrame() {
#ifdef BLIB_DEBUG
    if (renderer == nullptr) { BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Renderer"; }
#endif

    renderer->getTransferEngine().unregisterPerFrameTransfer(this, perFrame);
    perFrame = NotPerFrame;
    queued   = false;
}

} // namespace tfr
} // namespace rc
} // namespace bl

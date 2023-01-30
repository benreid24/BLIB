#include <BLIB/Render/Vulkan/Transferable.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
Transferable::Transferable()
: vulkanState(nullptr) {}

Transferable::Transferable(VulkanState& vs)
: vulkanState(&vs) {}

void Transferable::queueTransfer() {
#ifdef BLIB_DEBUG
    if (vulkanState == nullptr) {
        BL_LOG_CRITICAL << "Tried to queue transfer with nullptr Vulkan state";
    }
#endif

    vulkanState->transferEngine.queueTransfer(*this);
}

} // namespace render
} // namespace bl

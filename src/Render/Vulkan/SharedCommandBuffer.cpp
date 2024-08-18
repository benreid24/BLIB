#include <BLIB/Render/Vulkan/SharedCommandBuffer.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Vulkan/SharedCommandPool.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
SharedCommandBuffer::SharedCommandBuffer(SharedCommandBuffer&& move)
: owner(move.owner)
, buffer(move.buffer) {
    move.owner  = nullptr;
    move.buffer = nullptr;
}

SharedCommandBuffer::SharedCommandBuffer(SharedCommandPool* owner, VkFence fence,
                                         VkCommandBuffer buffer)
: owner(owner)
, fence(fence)
, buffer(buffer) {}

SharedCommandBuffer::~SharedCommandBuffer() {
    if (owner && buffer) {
        BL_LOG_ERROR << "Command buffer destroyed without being submitted";
        owner->mutex.unlock();
    }
}

void SharedCommandBuffer::submit() {
    if (owner && buffer) {
        owner->submit(*this);
        owner  = nullptr;
        buffer = nullptr;
    }
}

} // namespace vk
} // namespace rc
} // namespace bl

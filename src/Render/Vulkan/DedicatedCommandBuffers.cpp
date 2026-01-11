#include <BLIB/Render/Vulkan/DedicatedCommandBuffers.hpp>

#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
DedicatedCommandBuffers::DedicatedCommandBuffers()
: vs(nullptr) {}

DedicatedCommandBuffers::~DedicatedCommandBuffers() {
    if (vs) { destroy(); }
}

void DedicatedCommandBuffers::create(VulkanLayer& vulkanState) {
    vs = &vulkanState;
    payloads.init(*vs, [this](auto& p) { p.create(*vs); });
}

void DedicatedCommandBuffers::destroy() {
    if (vs) {
        payloads.cleanup([this](auto& p) { p.cleanup(*vs); });
        vs = nullptr;
    }
}

VkCommandBuffer DedicatedCommandBuffers::begin() {
    auto& p = payloads.current();

    vkCheck(vkWaitForFences(vs->getDevice(), 1, &p.fence, VK_TRUE, UINT64_MAX));
    vkCheck(vkResetFences(vs->getDevice(), 1, &p.fence));

    vkCheck(vkResetCommandBuffer(p.buffer, 0));

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkCheck(vkBeginCommandBuffer(p.buffer, &beginInfo));

    return p.buffer;
}

void DedicatedCommandBuffers::submit() {
    auto& p = payloads.current();
    vkCheck(vkEndCommandBuffer(p.buffer));

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &p.buffer;

    vs->submitCommandBuffer(submitInfo, p.fence);
}

void DedicatedCommandBuffers::Payload::create(VulkanLayer& vs) {
    pool = vs.createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                                VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = pool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    vkCheck(vkAllocateCommandBuffers(vs.getDevice(), &allocInfo, &buffer));

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCheck(vkCreateFence(vs.getDevice(), &fenceInfo, nullptr, &fence));
}

void DedicatedCommandBuffers::Payload::cleanup(VulkanLayer& vs) {
    vkDestroyFence(vs.getDevice(), fence, nullptr);
    vkDestroyCommandPool(vs.getDevice(), pool, nullptr);
}

} // namespace vk
} // namespace rc
} // namespace bl

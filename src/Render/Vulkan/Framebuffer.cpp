#include <BLIB/Render/Vulkan/Framebuffer.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
Framebuffer::Framebuffer(VulkanState& vulkanState)
: vulkanState(vulkanState)
, renderPass(nullptr) {
    commandPool = vulkanState.createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

Framebuffer::~Framebuffer() {
    if (renderPass) { cleanup(); }
}

void Framebuffer::create(VkRenderPass renderPass, VkExtent2D e, VkImageView* attachments,
                         std::uint32_t attachmentCount) {
    extent = e;

    // cleanup and block if recreating
    if (renderPass) {
        // TODO - may need to sync here?
        cleanup();
    }

    // create framebuffer
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = renderPass;
    framebufferInfo.attachmentCount = attachmentCount;
    framebufferInfo.pAttachments    = attachments;
    framebufferInfo.width           = extent.width;
    framebufferInfo.height          = extent.height;
    framebufferInfo.layers          = 1;
    if (vkCreateFramebuffer(vulkanState.device, &framebufferInfo, nullptr, &framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create framebuffer");
    }
}

VkCommandBuffer Framebuffer::beginRender() {
    // clear pool and command buffer
    vkResetCommandPool(vulkanState.device, commandPool, 0);

    // create command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(vulkanState.device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    // begin command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Optional
    beginInfo.pInheritanceInfo = nullptr;                                     // Optional
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    // begin render pass
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo;
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = renderPass;
    renderPassInfo.framebuffer       = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.clearValueCount   = 1;
    renderPassInfo.pClearValues      = &clearColor;
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // set scissor and viewport to default
    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(extent.width);
    viewport.height   = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    return commandBuffer;
}

void Framebuffer::finishRender(VkSemaphore waitSemaphore,
                               VkSemaphore finishedSemaphore, VkFence finishedFence) {
    // end render pass
    vkCmdEndRenderPass(commandBuffer);

    // end command buffer
    vkEndCommandBuffer(commandBuffer);

    // submit to queue
    VkSemaphore waitSemaphores[]      = {waitSemaphore};
    VkSemaphore signalSemaphores[]    = {finishedSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = std::size(waitSemaphores);
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.signalSemaphoreCount = std::size(signalSemaphores);
    submitInfo.pSignalSemaphores    = signalSemaphores;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &commandBuffer;
    if (vkQueueSubmit(vulkanState.graphicsQueue, 1, &submitInfo, finishedFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }
}

void Framebuffer::cleanup() {
    vkDestroyCommandPool(vulkanState.device, commandPool, nullptr);
    vkDestroyFramebuffer(vulkanState.device, framebuffer, nullptr);
    renderPass = nullptr;
}

} // namespace render
} // namespace bl

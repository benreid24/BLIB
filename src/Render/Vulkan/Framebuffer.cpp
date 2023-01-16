#include <BLIB/Render/Vulkan/Framebuffer.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
Framebuffer::Framebuffer(VulkanState& vulkanState)
: vulkanState(vulkanState)
, renderPass(nullptr)
, target(nullptr)
, framebuffer(nullptr) {}

Framebuffer::~Framebuffer() {
    if (renderPass) { cleanup(); }
}

void Framebuffer::create(VkRenderPass rp, const RenderFrame& frame) {
    // cleanup and block if recreating
    if (renderPass) {
        // TODO - may need to sync here?
        cleanup();
    }

    // copy create params
    renderPass = rp;
    target     = &frame;

    // create framebuffer
    VkImageView attachments[] = {frame.colorImageView()};
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = renderPass;
    framebufferInfo.attachmentCount = std::size(attachments);
    framebufferInfo.pAttachments    = attachments;
    framebufferInfo.width           = frame.renderExtent().width;
    framebufferInfo.height          = frame.renderExtent().height;
    framebufferInfo.layers          = 1;
    if (vkCreateFramebuffer(vulkanState.device, &framebufferInfo, nullptr, &framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create framebuffer");
    }
}

void Framebuffer::beginRender(VkCommandBuffer commandBuffer) {
#ifndef BLIB_DEBUG
    if (target == nullptr) {
        throw std::runtime_error("Framebuffer render started without specifiying target");
    }
#endif

    // begin render pass
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo;
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = renderPass;
    renderPassInfo.framebuffer       = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = target->renderExtent();
    renderPassInfo.clearValueCount   = 1;
    renderPassInfo.pClearValues      = &clearColor;
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // set scissor and viewport to default
    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(target->renderExtent().width);
    viewport.height   = static_cast<float>(target->renderExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = target->renderExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Framebuffer::finishRender(VkCommandBuffer commandBuffer) { vkCmdEndRenderPass(commandBuffer); }

void Framebuffer::cleanup() {
    vkDestroyFramebuffer(vulkanState.device, framebuffer, nullptr);
    renderPass = nullptr;
}

} // namespace render
} // namespace bl

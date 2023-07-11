#include <BLIB/Render/Vulkan/Framebuffer.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
Framebuffer::Framebuffer()
: vulkanState(nullptr)
, renderPass(nullptr)
, target(nullptr)
, framebuffer(nullptr) {}

Framebuffer::~Framebuffer() {
    if (renderPass) { cleanup(); }
}

void Framebuffer::create(VulkanState& vs, VkRenderPass rp, const AttachmentSet& frame) {
    vulkanState = &vs;

    // cleanup and block if recreating
    if (renderPass) { cleanup(); }

    // copy create params
    renderPass       = rp;
    target           = &frame;
    cachedAttachment = *frame.imageViews();

    // create framebuffer
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = renderPass;
    framebufferInfo.attachmentCount = frame.size();
    framebufferInfo.pAttachments    = frame.imageViews();
    framebufferInfo.width           = frame.renderExtent().width;
    framebufferInfo.height          = frame.renderExtent().height;
    framebufferInfo.layers          = 1;
    if (vkCreateFramebuffer(vulkanState->device, &framebufferInfo, nullptr, &framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create framebuffer");
    }
}

void Framebuffer::recreateIfChanged(const AttachmentSet& t) {
    if (*t.imageViews() != cachedAttachment) { create(*vulkanState, renderPass, t); }
}

void Framebuffer::beginRender(VkCommandBuffer commandBuffer, const VkRect2D& region,
                              VkClearValue* clearColors, std::uint32_t clearColorCount,
                              bool vp) const {
#ifdef BLIB_DEBUG
    if (target == nullptr) {
        throw std::runtime_error("Framebuffer gfx started without specifying target");
    }
#endif

    // begin render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass      = renderPass;
    renderPassInfo.framebuffer     = framebuffer;
    renderPassInfo.renderArea      = region;
    renderPassInfo.clearValueCount = clearColorCount;
    renderPassInfo.pClearValues    = clearColors;
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    if (vp) {
        VkViewport viewport;
        viewport.x        = static_cast<float>(region.offset.x);
        viewport.y        = static_cast<float>(region.offset.y);
        viewport.width    = static_cast<float>(region.extent.width);
        viewport.height   = static_cast<float>(region.extent.height);
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &region);
    }
}

void Framebuffer::finishRender(VkCommandBuffer commandBuffer) const {
    vkCmdEndRenderPass(commandBuffer);
}

void Framebuffer::cleanup() {
    vkDestroyFramebuffer(vulkanState->device, framebuffer, nullptr);
    renderPass = nullptr;
}

} // namespace vk
} // namespace gfx
} // namespace bl

#include <BLIB/Render/Vulkan/Framebuffer.hpp>

#include <BLIB/Render/Vulkan/RenderPass.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
Framebuffer::Framebuffer()
: vulkanState(nullptr)
, renderPass(nullptr)
, target(nullptr)
, framebuffer(nullptr)
, renderStartCount(0) {}

Framebuffer::~Framebuffer() {
    if (renderPass) { deferCleanup(); }
}

void Framebuffer::create(VulkanLayer& vs, const RenderPass* rp, const AttachmentSet& frame) {
    vulkanState = &vs;

    // cleanup and block if recreating
    if (renderPass) { deferCleanup(); }

    // copy create params
    renderPass       = rp;
    target           = &frame;
    cachedAttachment = frame.getImageViews()[0];

    // create framebuffer
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = renderPass->rawPass();
    framebufferInfo.attachmentCount = frame.getAttachmentCount();
    framebufferInfo.pAttachments    = frame.getImageViews();
    framebufferInfo.width           = frame.getRenderExtent().width;
    framebufferInfo.height          = frame.getRenderExtent().height;
    framebufferInfo.layers          = frame.getLayerCount();
    if (vkCreateFramebuffer(vulkanState->getDevice(), &framebufferInfo, nullptr, &framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create framebuffer");
    }
}

void Framebuffer::recreateIfChanged(const AttachmentSet& t) {
    // TODO - need to check all views?
    if (t.getImageViews()[0] != cachedAttachment) { create(*vulkanState, renderPass, t); }
}

void Framebuffer::beginRender(VkCommandBuffer commandBuffer, const VkRect2D& region,
                              const VkClearValue* clearColors, std::uint32_t clearColorCount,
                              bool vp, VkRenderPass rpo, bool shouldClear) {
#ifdef BLIB_DEBUG
    if (target == nullptr) {
        throw std::runtime_error("Framebuffer render started without specifying target");
    }
#endif

    bool cleared = false;
    if (renderStartCount == 0) {
        // begin render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass      = rpo != nullptr ? rpo : renderPass->rawPass();
        renderPassInfo.framebuffer     = framebuffer;
        renderPassInfo.renderArea      = region;
        renderPassInfo.clearValueCount = clearColorCount;
        renderPassInfo.pClearValues    = clearColors;
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        cleared = renderPass->getCreateParams().isClearedOnStart();
    }
    if (shouldClear && !cleared) {
        // if we have resolve attachments we need to avoid clearing them
        std::uint32_t colorCount = clearColorCount;
        if (target->getOutputIndex() > 0) {
            colorCount = std::min(colorCount, target->getOutputIndex());
        }

        // if we have a depth attachment we will handle it outside of the loop
        const bool hasDepth =
            target->getImageAspect(target->getAttachmentCount() - 1) & VK_IMAGE_ASPECT_DEPTH_BIT;
        const std::uint32_t attachmentCount = hasDepth ? colorCount + 1 : colorCount;

        VkClearAttachment attachments[AttachmentSet::MaxAttachments]{};
        for (unsigned int i = 0; i < colorCount; ++i) {
            attachments[i].aspectMask      = target->getImageAspects()[i];
            attachments[i].colorAttachment = i;
            attachments[i].clearValue      = clearColors[i];
        }
        if (hasDepth) {
            attachments[colorCount].aspectMask =
                target->getImageAspect(target->getAttachmentCount() - 1);
            attachments[colorCount].colorAttachment = VK_ATTACHMENT_UNUSED;
            attachments[colorCount].clearValue      = clearColors[clearColorCount - 1];
        }

        VkClearRect rect;
        rect.rect           = region;
        rect.baseArrayLayer = 0;
        rect.layerCount     = target->getLayerCount();

        vkCmdClearAttachments(commandBuffer, attachmentCount, attachments, 1, &rect);
    }

    vkCmdSetScissor(commandBuffer, 0, 1, &region);

    if (vp) {
        VkViewport viewport;
        viewport.x        = static_cast<float>(region.offset.x);
        viewport.y        = static_cast<float>(region.offset.y);
        viewport.width    = static_cast<float>(region.extent.width);
        viewport.height   = static_cast<float>(region.extent.height);
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    ++renderStartCount;
}

void Framebuffer::finishRender(VkCommandBuffer commandBuffer) {
#ifdef BLIB_DEBUG
    if (renderStartCount == 0) {
        throw std::runtime_error("Framebuffer render finished without starting render");
    }
#endif

    if (--renderStartCount == 0) { vkCmdEndRenderPass(commandBuffer); }
}

void Framebuffer::cleanup() {
    vkDestroyFramebuffer(vulkanState->getDevice(), framebuffer, nullptr);
    renderPass = nullptr;
}

void Framebuffer::deferCleanup() {
    if (renderPass) {
        vulkanState->getCleanupManager().add(
            [device = vulkanState->getDevice(), fb = framebuffer]() {
                vkDestroyFramebuffer(device, fb, nullptr);
            });
        renderPass = nullptr;
    }
}

} // namespace vk
} // namespace rc
} // namespace bl

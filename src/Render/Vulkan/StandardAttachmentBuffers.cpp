#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
StandardAttachmentBuffers::StandardAttachmentBuffers()
: owner(nullptr) {}

void StandardAttachmentBuffers::create(VulkanState& vs, const VkExtent2D& size,
                                       VkFormat colorFormat) {
    owner = &vs;
    attachments.setRenderExtent(size);

    colorAttachment.create(
        vs, colorFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, size);
    depthAttachment.create(
        vs, findDepthFormat(vs), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, size);
    attachments.setAttachments(colorAttachment.image(),
                               colorAttachment.view(),
                               depthAttachment.image(),
                               depthAttachment.view());
}

void StandardAttachmentBuffers::destroy() {
    if (owner != nullptr) {
        colorAttachment.destroy();
        depthAttachment.destroy();
        owner = nullptr;
    }
}

void StandardAttachmentBuffers::deferDestroy() {
    if (owner != nullptr) {
        colorAttachment.deferDestroy();
        depthAttachment.deferDestroy();
        owner = nullptr;
    }
}

VkFormat StandardAttachmentBuffers::findDepthFormat(VulkanState& vs) {
    return vs.findSupportedFormat(
        {VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

} // namespace vk
} // namespace rc
} // namespace bl

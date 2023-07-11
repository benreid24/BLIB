#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace gfx
{
namespace vk
{
StandardAttachmentBuffers::StandardAttachmentBuffers()
: owner(nullptr) {}

StandardAttachmentBuffers::~StandardAttachmentBuffers() { destroy(); }

void StandardAttachmentBuffers::create(VulkanState& vs, const VkExtent2D& size) {
    owner = &vs;
    attachments.setRenderExtent(size);

    colorAttachment.create(vs,
                           DefaultColorFormat,
                           VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                           size);
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

VkFormat StandardAttachmentBuffers::findDepthFormat(VulkanState& vs) {
    return vs.findSupportedFormat(
        {VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

} // namespace vk
} // namespace gfx
} // namespace bl

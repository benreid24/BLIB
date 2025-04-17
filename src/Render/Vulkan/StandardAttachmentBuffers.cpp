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

    colorAttachment.create(vs,
                           Image::Type::Image2D,
                           colorFormat,
                           VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                           size,
                           VK_IMAGE_ASPECT_COLOR_BIT,
                           VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
    depthAttachment.create(vs,
                           Image::Type::Image2D,
                           findDepthFormat(vs),
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           size,
                           VK_IMAGE_ASPECT_DEPTH_BIT,
                           VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
    attachments.setAttachments(colorAttachment.getImage(),
                               colorAttachment.getView(),
                               depthAttachment.getImage(),
                               depthAttachment.getView());
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

VkImageAspectFlags VulkanState::guessImageAspect(VkFormat, VkImageUsageFlags usage) {
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) { return VK_IMAGE_ASPECT_DEPTH_BIT; }
    return VK_IMAGE_ASPECT_COLOR_BIT;
}

} // namespace vk
} // namespace rc
} // namespace bl

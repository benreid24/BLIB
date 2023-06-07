#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>

namespace bl
{
namespace render
{
namespace vk
{
StandardAttachmentBuffers::StandardAttachmentBuffers()
: owner(nullptr) {}

StandardAttachmentBuffers::~StandardAttachmentBuffers() { destroy(); }

void StandardAttachmentBuffers::create(VulkanState& vs, const VkExtent2D& size) {
    owner              = &vs;
    attachments.extent = size;

    colorAttachment.create(vs,
                           VK_FORMAT_R8G8B8A8_SRGB,
                           VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                           size);
    attachments.imageHandles[StandardAttachmentSet::ColorIndex]     = colorAttachment.image();
    attachments.imageViewHandles[StandardAttachmentSet::ColorIndex] = colorAttachment.view();

    depthAttachment.create(
        vs, findDepthFormat(vs), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, size);
    attachments.imageHandles[StandardAttachmentSet::DepthIndex]     = depthAttachment.image();
    attachments.imageViewHandles[StandardAttachmentSet::DepthIndex] = depthAttachment.view();
}

void StandardAttachmentBuffers::destroy() {
    if (owner != nullptr) {
        colorAttachment.destroy();
        depthAttachment.destroy();
        owner = nullptr;
    }
}

void StandardAttachmentBuffers::prepareForSampling(VkCommandBuffer cb) {
    // TODO - not sure that any of this is correct
    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = attachments.colorImage();
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.srcAccessMask                   = 0;
    barrier.dstAccessMask                   = 0;
    vkCmdPipelineBarrier(cb,
                         VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, // wait for rendering to complete
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // block rendering
                         0,                                  // no dependency flags
                         0,                                  // 0 memory barriers
                         nullptr,
                         0, // 0 buffer memory barriers
                         nullptr,
                         1,
                         &barrier);
}

VkFormat StandardAttachmentBuffers::findDepthFormat(VulkanState& vs) {
    return vs.findSupportedFormat(
        {VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

} // namespace vk
} // namespace render
} // namespace bl

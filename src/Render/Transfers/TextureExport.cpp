#include <BLIB/Render/Transfers/TextureExport.hpp>

#include <BLIB/Render/Transfers/TextureExporter.hpp>
#include <BLIB/Render/Vulkan/VkCheck.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace rc
{
namespace tfr
{
namespace
{
constexpr std::uint64_t TimeoutNs = 10ull * 1000ull * 1000ull * 1000ull;
}

TextureExport::TextureExport(vk::VulkanState& vs, TextureExporter& owner, VkImage src,
                             VkImageLayout srcLayout, VkExtent3D srcExtent,
                             VkImageAspectFlags srcAspect)
: vulkanState(vs)
, owner(owner)
, srcImage(src)
, srcLayout(srcLayout)
, srcExtent(srcExtent)
, srcAspect(srcAspect) {
    commandPool = vulkanState.createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    VkCommandBufferAllocateInfo cbAllocInfo{};
    cbAllocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbAllocInfo.commandPool        = commandPool;
    cbAllocInfo.commandBufferCount = 1;
    vkCheck(vkAllocateCommandBuffers(vulkanState.device, &cbAllocInfo, &commandBuffer));

    VkFenceCreateInfo createFence{};
    createFence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createFence.flags = 0;
    vkCheck(vkCreateFence(vulkanState.device, &createFence, nullptr, &fence));

    const VkDeviceSize size = srcExtent.width * srcExtent.height * 4;
    vulkanState.createBuffer(size,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VMA_ALLOCATION_CREATE_MAPPED_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             &destBuffer,
                             &destAlloc,
                             &destAllocInfo);
}

TextureExport::~TextureExport() {
    vmaDestroyBuffer(vulkanState.vmaAllocator, destBuffer, destAlloc);
    vkDestroyFence(vulkanState.device, fence, nullptr);
    vkDestroyCommandPool(vulkanState.device, commandPool, nullptr);
}

bool TextureExport::imageReady() const {
    return vkWaitForFences(vulkanState.device, 1, &fence, VK_TRUE, 0) == VK_SUCCESS;
}

void TextureExport::wait() {
    vkCheck(vkWaitForFences(vulkanState.device, 1, &fence, VK_TRUE, TimeoutNs));
}

void TextureExport::copyImage(sf::Image& dst) {
    if (!imageReady()) {
        BL_LOG_WARN << "copyImage() called before export is ready";
        wait();
    }

    dst.create(
        srcExtent.width, srcExtent.height, static_cast<sf::Uint8*>(destAllocInfo.pMappedData));
}

void TextureExport::release() { owner.release(this); }

void TextureExport::performCopy() {
    // Begin recording commands
    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkCheck(vkBeginCommandBuffer(commandBuffer, &begin));

    // transition image to copy source layout
    vulkanState.transitionImageLayout(
        commandBuffer, srcImage, srcLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    // copy image to buffer
    VkBufferImageCopy copyCmd{};
    copyCmd.bufferOffset                    = 0;
    copyCmd.bufferRowLength                 = srcExtent.width;
    copyCmd.bufferImageHeight               = srcExtent.height;
    copyCmd.imageOffset                     = {0, 0, 0};
    copyCmd.imageExtent                     = srcExtent;
    copyCmd.imageSubresource.aspectMask     = srcAspect;
    copyCmd.imageSubresource.mipLevel       = 0;
    copyCmd.imageSubresource.baseArrayLayer = 0;
    copyCmd.imageSubresource.layerCount     = 1;
    vkCmdCopyImageToBuffer(
        commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destBuffer, 1, &copyCmd);

    // insert barrier to block until copy is complete
    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout                       = srcLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = srcImage;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.srcAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
    barrier.dstAccessMask                   = VK_ACCESS_MEMORY_WRITE_BIT;
    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    // submit commands
    vkCheck(vkEndCommandBuffer(commandBuffer));
    VkSubmitInfo submit{};
    submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pCommandBuffers    = &commandBuffer;
    submit.commandBufferCount = 1;
    vulkanState.submitCommandBuffer(submit, fence);
}

} // namespace tfr
} // namespace rc
} // namespace bl

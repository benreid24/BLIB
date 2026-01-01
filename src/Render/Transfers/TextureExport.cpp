#include <BLIB/Render/Transfers/TextureExport.hpp>

#include <BLIB/Render/Transfers/TextureExporter.hpp>
#include <BLIB/Render/Vulkan/VkCheck.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

namespace bl
{
namespace rc
{
namespace tfr
{
namespace
{
constexpr std::uint64_t TimeoutNs = 10ull * 1000ull * 1000ull * 1000ull;
constexpr VkFormat OutputFormat   = VK_FORMAT_R8G8B8A8_UNORM;

bool requiresSwizzle(VkFormat format) {
    return format == VK_FORMAT_B8G8R8A8_SRGB || format == VK_FORMAT_B8G8R8A8_UNORM ||
           format == VK_FORMAT_B8G8R8A8_SNORM;
}
} // namespace

TextureExport::TextureExport(vk::VulkanLayer& vs, TextureExporter& owner, VkImage src,
                             VkImageLayout srcLayout, VkExtent3D srcExtent,
                             VkImageAspectFlags srcAspect, VkFormat srcFormat)
: vulkanState(vs)
, owner(owner)
, requiresManualConversion(false)
, mapped(nullptr)
, srcImage(src)
, srcLayout(srcLayout)
, srcExtent(srcExtent)
, srcAspect(srcAspect)
, srcFormat(srcFormat)
, inProgress(true) {}

TextureExport::~TextureExport() {
    if (mapped) { vmaUnmapMemory(vulkanState.getVmaAllocator(), destAlloc); }
    vmaDestroyImage(vulkanState.getVmaAllocator(), destImage, destAlloc);
    vkDestroyFence(vulkanState.getDevice(), fence, nullptr);
    vkDestroyCommandPool(vulkanState.getDevice(), commandPool, nullptr);
}

bool TextureExport::imageReady() const { return !inProgress; }

void TextureExport::wait() {
    if (inProgress) {
        std::unique_lock lock(progressMutex);
        cv.wait(lock);
    }
}

bool TextureExport::checkComplete() {
    if (inProgress) {
        std::unique_lock lock(progressMutex);
        if (vkWaitForFences(vulkanState.getDevice(), 1, &fence, VK_TRUE, 0) == VK_SUCCESS) {
            inProgress = false;
            cv.notify_all();
            return true;
        }
        return false;
    }
    return true;
}

void TextureExport::copyImage(sf::Image& dst) {
    if (!imageReady()) {
        BL_LOG_WARN << "copyImage() called before export is ready";
        wait();
    }

    if (!mapped) { vmaMapMemory(vulkanState.getVmaAllocator(), destAlloc, &mapped); }

    dst.create(srcExtent.width, srcExtent.height, static_cast<sf::Uint8*>(mapped));
    if (requiresManualConversion) {
        for (unsigned int y = 0; y < dst.getSize().y; ++y) {
            for (unsigned int x = 0; x < dst.getSize().x; ++x) {
                const sf::Color c = dst.getPixel(x, y);
                dst.setPixel(x, y, {c.b, c.g, c.r, c.a});
            }
        }
    }
}

void TextureExport::release() { owner.release(this); }

void TextureExport::performCopy() {
    // create resources
    commandPool = vulkanState.createCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    VkCommandBufferAllocateInfo cbAllocInfo{};
    cbAllocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbAllocInfo.commandPool        = commandPool;
    cbAllocInfo.commandBufferCount = 1;
    vkCheck(vkAllocateCommandBuffers(vulkanState.getDevice(), &cbAllocInfo, &commandBuffer));

    VkFenceCreateInfo createFence{};
    createFence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createFence.flags = 0;
    vkCheck(vkCreateFence(vulkanState.getDevice(), &createFence, nullptr, &fence));

    // create staging image to copy to
    vulkanState.createImage(srcExtent.width,
                            srcExtent.height,
                            1,
                            OutputFormat,
                            VK_IMAGE_TILING_LINEAR,
                            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                            0,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            &destImage,
                            &destAlloc,
                            &destAllocInfo,
                            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);
    // Begin recording commands
    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkCheck(vkBeginCommandBuffer(commandBuffer, &begin));

    // transition image to copy source layout
    vulkanState.transitionImageLayout(
        commandBuffer, srcImage, srcLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    // transition dest image to dest layout
    vulkanState.transitionImageLayout(
        commandBuffer, destImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    const VkFormatProperties srcFormatProps  = vulkanState.getFormatProperties(srcFormat);
    const VkFormatProperties convFormatProps = vulkanState.getFormatProperties(OutputFormat);

    // blit if supported otherwise copy
    if ((srcFormatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) != 0 &&
        (convFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) != 0) {
        const VkOffset3D blitSize{static_cast<std::int32_t>(srcExtent.width),
                                  static_cast<std::int32_t>(srcExtent.height),
                                  1};
        VkImageBlit imageBlitRegion{};
        imageBlitRegion.srcSubresource.aspectMask = srcAspect;
        imageBlitRegion.srcSubresource.layerCount = 1;
        imageBlitRegion.srcOffsets[1]             = blitSize;
        imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlitRegion.dstSubresource.layerCount = 1;
        imageBlitRegion.dstOffsets[1]             = blitSize;

        vkCmdBlitImage(commandBuffer,
                       srcImage,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       destImage,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &imageBlitRegion,
                       VK_FILTER_NEAREST);
    }
    else {
        // check if the format requires swizzling
        requiresManualConversion = requiresSwizzle(srcFormat);

        VkImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = srcAspect;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent                    = srcExtent;

        vkCmdCopyImage(commandBuffer,
                       srcImage,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       destImage,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &imageCopyRegion);
    }

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
    barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
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

    // transition dest image to general format
    VkImageMemoryBarrier destBarrier{};
    destBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    destBarrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    destBarrier.newLayout                       = VK_IMAGE_LAYOUT_GENERAL;
    destBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    destBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    destBarrier.image                           = destImage;
    destBarrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    destBarrier.subresourceRange.baseMipLevel   = 0;
    destBarrier.subresourceRange.levelCount     = 1;
    destBarrier.subresourceRange.baseArrayLayer = 0;
    destBarrier.subresourceRange.layerCount     = 1;
    destBarrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
    destBarrier.dstAccessMask                   = VK_ACCESS_MEMORY_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &destBarrier);

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

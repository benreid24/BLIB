#include <BLIB/Render/Scenes/Texture.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>

namespace bl
{
namespace render
{
Texture::Texture()
: image(nullptr)
, memory(nullptr)
, memoryOffset(0)
, memorySize(0)
, view(nullptr)
, sampler(nullptr)
, size(0, 0)
, sizeF(0.f, 0.f) {}

void Texture::createFromContentsAndQueue(VulkanState& vs) {
    const sf::Image& src = altImg ? *altImg : *transferImg;

    size.x  = src.getSize().x;
    size.y  = src.getSize().y;
    sizeF.x = static_cast<float>(size.x);
    sizeF.y = static_cast<float>(size.y);

    // TODO - avoid single allocations per texture
    memorySize   = src.getSize().x * src.getSize().y * sizeof(sf::Color);
    memoryOffset = 0;
    vs.createImage(src.getSize().x,
                   src.getSize().y,
                   VK_FORMAT_R8G8B8A8_UNORM,
                   VK_IMAGE_TILING_OPTIMAL,
                   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                   image,
                   memory);
    view = vs.createImageView(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    // barrier + transition to dest format. is this best way?
    vs.transitionImageLayout(image,
                             VK_FORMAT_R8G8B8A8_UNORM,
                             VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vs.transferEngine.queueTransfer(*this);
}

void Texture::executeTransfer(VkCommandBuffer cb, TransferEngine& engine) {
    const sf::Image& src = altImg ? *altImg : *transferImg;

    // create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    engine.createStagingBuffer(memorySize, stagingBuffer, stagingMemory);

    // copy data to staging buffer
    void* data;
    vkMapMemory(engine.device(), stagingMemory, 0, memorySize, 0, &data);
    std::memcpy(data, src.getPixelsPtr(), memorySize);
    vkUnmapMemory(engine.device(), stagingMemory);

    // issue copy command
    VkBufferImageCopy copyInfo{};
    copyInfo.bufferOffset                    = 0;
    copyInfo.bufferRowLength                 = 0;
    copyInfo.bufferImageHeight               = 0;
    copyInfo.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copyInfo.imageSubresource.mipLevel       = 0;
    copyInfo.imageSubresource.baseArrayLayer = 0;
    copyInfo.imageSubresource.layerCount     = 1;
    copyInfo.imageOffset                     = {0, 0, 0};
    copyInfo.imageExtent                     = {size.x, size.y, 1};
    vkCmdCopyBufferToImage(
        cb, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);

    // insert pipeline barrier
    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
    engine.registerImageBarrier(barrier);

    // cleanup
    if (!altImg) { transferImg.release(); }
    else { altImg = nullptr; }
}

void Texture::cleanup(VulkanState& vs) {
    vkDestroyImageView(vs.device, view, nullptr);
    vkDestroyImage(vs.device, image, nullptr);
    vkFreeMemory(vs.device, memory, nullptr);
}

} // namespace render
} // namespace bl

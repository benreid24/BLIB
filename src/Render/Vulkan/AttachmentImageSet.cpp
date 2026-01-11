#include <BLIB/Render/Vulkan/AttachmentImageSet.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
AttachmentImageSet::AttachmentImageSet()
: owner(nullptr) {}

void AttachmentImageSet::create(Renderer& renderer, unsigned int count, const VkExtent2D& size,
                                const vk::SemanticTextureFormat* bufferFormats,
                                const VkImageUsageFlags* usages, VkSampleCountFlagBits samples,
                                std::uint32_t firstResolveAttachment) {
    owner = &renderer;
    attachments.setRenderExtent(size);

    auto commandBuffer = owner->getSharedCommandPool().createBuffer();
    std::array<VkImageAspectFlags, MaxBufferCount> aspects;
    for (std::uint32_t i = 0; i < count; ++i) {
        formats[i] = bufferFormats[i];
        aspects[i] = VulkanLayer::guessImageAspect(bufferFormats[i]);
        buffers[i].create(renderer,
                          {.type   = ImageOptions::Type::Image2D,
                           .format = renderer.getTextureFormatManager().getFormat(bufferFormats[i]),
                           .usage  = usages[i],
                           .extent = size,
                           .aspect = aspects[i],
                           .allocFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                           .samples    = samples});
        images[i] = buffers[i].getImage();
        views[i]  = buffers[i].getView();
        buffers[i].clearAndTransition(commandBuffer,
                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      {.color = {{0.f, 0.f, 0.f, 1.f}}});
    }
    commandBuffer.submit();

    attachments.init(
        count, images.data(), views.data(), aspects.data(), size, buffers[0].getLayerCount());
    attachments.setOutputIndex(firstResolveAttachment);
}

void AttachmentImageSet::destroy() {
    if (owner != nullptr) {
        owner = nullptr;
        for (auto& b : buffers) { b.destroy(); }
    }
}

void AttachmentImageSet::deferDestroy() {
    if (owner != nullptr) {
        owner = nullptr;
        for (auto& b : buffers) { b.deferDestroy(); }
    }
}

bool AttachmentImageSet::recreateForFormatChange() {
    if (owner == nullptr) { return false; }

    bool recreated = false;
    std::array<VkImageAspectFlags, MaxBufferCount> aspects;
    for (std::uint32_t i = 0; i < attachments.getAttachmentCount(); ++i) {
        const VkFormat actualFormat = owner->getTextureFormatManager().getFormat(formats[i]);
        if (buffers[i].getFormat() != actualFormat) {
            recreated  = true;
            aspects[i] = VulkanLayer::guessImageAspect(formats[i]);
            buffers[i].create(*owner,
                              {.type       = ImageOptions::Type::Image2D,
                               .format     = actualFormat,
                               .usage      = buffers[i].getUsage(),
                               .extent     = attachments.getRenderExtent(),
                               .aspect     = aspects[i],
                               .allocFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                               .samples    = buffers[i].getSampleCount()});
            images[i] = buffers[i].getImage();
            views[i]  = buffers[i].getView();
        }
    }
    if (recreated) {
        attachments.init(attachments.getAttachmentCount(),
                         images.data(),
                         views.data(),
                         aspects.data(),
                         attachments.getRenderExtent(),
                         buffers[0].getLayerCount());
    }
    return recreated;
}

} // namespace vk
} // namespace rc
} // namespace bl
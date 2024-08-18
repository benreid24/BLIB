#include <BLIB/Render/Vulkan/TextureDoubleBuffered.hpp>

#include <BLIB/Render/Resources/BindlessTextureArray.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
TextureDoubleBuffered::TextureDoubleBuffered() {}

void TextureDoubleBuffered::create(const glm::u32vec2& s) {
    updateSize(s);

    images.init(*vulkanState, [this, &s](auto& img) {
        vulkanState->createImage(s.x,
                                 s.y,
                                 vk::StandardAttachmentBuffers::DefaultColorFormat,
                                 VK_IMAGE_TILING_OPTIMAL,
                                 VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                     VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                 &img.image,
                                 &img.alloc,
                                 &img.allocInfo);
        img.view = vulkanState->createImageView(img.image,
                                                vk::StandardAttachmentBuffers::DefaultColorFormat,
                                                VK_IMAGE_ASPECT_COLOR_BIT);
        vulkanState->transitionImageLayout(
            img.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    });
}

void TextureDoubleBuffered::resize(const glm::u32vec2& s) {
    // defer delete of old images
    images.visit([this](auto& img) {
        vulkanState->cleanupManager.add(
            [vulkanState = vulkanState, view = img.view, image = img.image, alloc = img.alloc]() {
                vkDestroyImageView(vulkanState->device, view, nullptr);
                vmaDestroyImage(vulkanState->vmaAllocator, image, alloc);
            });
    });

    create(s);
    updateDescriptors();
}

void TextureDoubleBuffered::executeTransfer(VkCommandBuffer, tfr::TransferContext&) {
    // noop
}

TextureDoubleBuffered& TextureDoubleBuffered::operator=(const Texture& t) {
    images.visit([&t](auto& img) {
        img.image = t.getImage();
        img.view  = t.getView();
    });
    TextureBase::operator=(static_cast<const TextureBase&>(t));
    return *this;
}

void TextureDoubleBuffered::cleanup() {
    images.cleanup([this](auto& img) {
        vkDestroyImageView(vulkanState->device, img.view, nullptr);
        vmaDestroyImage(vulkanState->vmaAllocator, img.image, img.alloc);
    });
}

VkImage TextureDoubleBuffered::getCurrentImage() const { return images.current().image; }

VkImageLayout TextureDoubleBuffered::getCurrentImageLayout() const {
    // return the layout that we will be in at the end of the current frame
    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

VkFormat TextureDoubleBuffered::getFormat() const {
    return vk::StandardAttachmentBuffers::DefaultColorFormat;
}

} // namespace vk
} // namespace rc
} // namespace bl

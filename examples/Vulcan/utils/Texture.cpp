#include "Texture.hpp"
#include "../Renderer.hpp"

VkDevice Texture::device = nullptr;

namespace
{
float maxAnisotropy = -1.f;
}

Texture::Texture()
: textureImage(nullptr)
, textureImageMemory(nullptr)
, textureImageView(nullptr)
, textureSampler(nullptr) {}

Texture::~Texture() {
    if (textureSampler) { vkDestroySampler(device, textureSampler, nullptr); }
    vkDestroyImageView(device, textureImageView, nullptr);
    vkDestroyImage(device, textureImage, nullptr);
    vkFreeMemory(device, textureImageMemory, nullptr);
}

void Texture::createSampler(VkSamplerAddressMode addressMode, VkFilter magFilter,
                            VkFilter minFilter) {
    createSampler(addressMode, addressMode, addressMode, magFilter, minFilter);
}

void Texture::createSampler(VkSamplerAddressMode addressModeU, VkSamplerAddressMode addressModeV,
                            VkSamplerAddressMode addressModeW, VkFilter magFilter,
                            VkFilter minFilter) {
    if (textureSampler) { vkDestroySampler(device, textureSampler, nullptr); }

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter               = magFilter;
    samplerInfo.minFilter               = minFilter;
    samplerInfo.addressModeU            = addressModeU;
    samplerInfo.addressModeV            = addressModeV;
    samplerInfo.addressModeW            = addressModeW;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.anisotropyEnable        = VK_TRUE;
    samplerInfo.maxAnisotropy           = maxAnisotropy;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias              = 0.0f;
    samplerInfo.minLod                  = 0.0f;
    samplerInfo.maxLod                  = 0.0f;
    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler");
    }
}

TextureLoader::TextureLoader(Renderer& r)
: renderer(r) {}

bool TextureLoader::load(const std::string&, const char* buffer, std::size_t len, std::istream&,
                         Texture& result) {
    Texture::device = renderer.state.device;
    if (maxAnisotropy < 0.f) {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(renderer.state.physicalDevice, &properties);
        maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    }

    sf::Image img;
    if (!img.loadFromMemory(buffer, len)) return false;

    // staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    const VkDeviceSize imageSize = img.getSize().x * img.getSize().y * 4;
    renderer.state.createBuffer(imageSize,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                stagingBuffer,
                                stagingBufferMemory);

    // copy to staging
    void* data;
    vkMapMemory(renderer.state.device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, img.getPixelsPtr(), static_cast<std::size_t>(imageSize));
    vkUnmapMemory(renderer.state.device, stagingBufferMemory);

    // create the image itself
    renderer.state.createImage(img.getSize().x,
                               img.getSize().y,
                               VK_FORMAT_R8G8B8A8_SRGB,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               result.textureImage,
                               result.textureImageMemory);

    // ensure proper layout for transfer
    renderer.state.transitionImageLayout(result.textureImage,
                                         VK_FORMAT_R8G8B8A8_SRGB,
                                         VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // copy data to GPU
    renderer.state.copyBufferToImage(stagingBuffer,
                                     result.textureImage,
                                     static_cast<std::uint32_t>(img.getSize().x),
                                     static_cast<std::uint32_t>(img.getSize().y));

    // optimize layout for shader sampling
    renderer.state.transitionImageLayout(result.textureImage,
                                         VK_FORMAT_R8G8B8A8_SRGB,
                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // create image view
    result.textureImageView =
        renderer.state.createImageView(result.textureImage, VK_FORMAT_R8G8B8A8_SRGB);

    // cleanup
    vkDestroyBuffer(renderer.state.device, stagingBuffer, nullptr);
    vkFreeMemory(renderer.state.device, stagingBufferMemory, nullptr);
    return true;
}

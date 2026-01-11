#include <BLIB/Render/Vulkan/TextureFormatManager.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Vulkan/CommonTextureFormats.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
TextureFormatManager::TextureFormatManager()
: owner(nullptr)
, currentColorFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
, currentDepthFormat(VK_FORMAT_D32_SFLOAT)
, currentDepthStencilFormat(VK_FORMAT_D24_UNORM_S8_UINT) {}

void TextureFormatManager::init(Renderer& renderer) {
    owner = &renderer.vulkanState();
    emitter.connect(renderer.getSignalChannel());
}

VkFormat TextureFormatManager::getFormat(SemanticTextureFormat format) {
    switch (format) {
    case SemanticTextureFormat::Color:
        return currentColorFormat;
    case SemanticTextureFormat::Depth:
        return currentDepthFormat;
    case SemanticTextureFormat::DepthStencil:
        return currentDepthStencilFormat;
    case SemanticTextureFormat::LinearRGBA32Bit:
        return CommonTextureFormats::LinearRGBA32Bit;
    case SemanticTextureFormat::SRGBA32Bit:
        return CommonTextureFormats::SRGBA32Bit;
    case SemanticTextureFormat::SFloatR16G16B16A16:
        return CommonTextureFormats::HDRColor;
    case SemanticTextureFormat::HighPrecisionColor:
        return owner->findHighPrecisionFormat();
    case SemanticTextureFormat::SingleChannelUnorm8:
        return CommonTextureFormats::SingleChannelUnorm8;
    default:
        throw std::runtime_error("Unknown texture format");
    }
}

void TextureFormatManager::setFormat(SemanticTextureFormat semantic, VkFormat actual) {
    switch (semantic) {
    case SemanticTextureFormat::Color:
        currentColorFormat = actual;
        break;
    case SemanticTextureFormat::Depth:
        currentDepthFormat = actual;
        break;
    case SemanticTextureFormat::DepthStencil:
        currentDepthStencilFormat = actual;
        break;
    default:
        throw std::runtime_error("Only semantic formats can be changed");
    }

    emitter.emit<event::TextureFormatChanged>({*this, semantic, actual});
}

} // namespace vk
} // namespace rc
} // namespace bl

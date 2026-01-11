#ifndef BLIB_RENDER_VULKAN_TEXTUREFORMATMANAGER_HPP
#define BLIB_RENDER_VULKAN_TEXTUREFORMATMANAGER_HPP

#include <BLIB/Render/Events/TextureFormatChanged.hpp>
#include <BLIB/Render/Vulkan/SemanticTextureFormat.hpp>
#include <BLIB/Signals/Emitter.hpp>
#include <BLIB/Vulkan.hpp>
#include <type_traits>

namespace bl
{
namespace rc
{
class Renderer;
class Settings;

namespace vk
{
struct VulkanLayer;

/**
 * @brief Common texture formats
 *
 * @ingroup Renderer
 */
struct TextureFormatManager {
    /**
     * @brief Creates a new texture format manager with default formats
     */
    TextureFormatManager();

    /**
     * @brief Returns the VkFormat for the given semantic format
     *
     * @param format The semantic format to get
     * @return The actual VkFormat to use
     */
    VkFormat getFormat(SemanticTextureFormat format);

private:
    VulkanLayer* owner;
    sig::Emitter<event::TextureFormatChanged> emitter;
    VkFormat currentColorFormat;
    VkFormat currentDepthFormat;
    VkFormat currentDepthStencilFormat;

    void init(Renderer& renderer);
    void setFormat(SemanticTextureFormat semantic, VkFormat actual);

    friend class bl::rc::Settings;
    friend class bl::rc::Renderer;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

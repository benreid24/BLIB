#ifndef BLIB_RENDER_EVENTS_TEXTUREFORMATCHANGED_HPP
#define BLIB_RENDER_EVENTS_TEXTUREFORMATCHANGED_HPP

#include <BLIB/Render/Vulkan/SemanticTextureFormat.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
struct TextureFormatManager;
}
namespace event
{
/**
 * @brief Fired when the underlying VkFormat for a semantic texture format changes
 *
 * @ingroup Renderer
 */
struct TextureFormatChanged {
    vk::TextureFormatManager& formatManager;
    vk::SemanticTextureFormat semanticFormat;
    VkFormat newFormat;
};

} // namespace event
} // namespace rc
} // namespace bl

#endif

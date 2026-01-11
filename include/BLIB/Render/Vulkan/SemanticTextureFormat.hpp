#ifndef BLIB_RENDER_VULKAN_SEMANTICTEXTUREFORMAT_HPP
#define BLIB_RENDER_VULKAN_SEMANTICTEXTUREFORMAT_HPP

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Represents a semantic texture format. Underlying formats can be changed at runtime
 *
 * @ingroup Renderer
 */
enum struct SemanticTextureFormat {
    // Semantic formats that can be changed
    Color,
    Depth,
    DepthStencil,

    // Specific formats for specific uses
    LinearRGBA32Bit,
    SRGBA32Bit,
    SFloatR16G16B16A16,
    HighPrecisionColor,
    SingleChannelUnorm8,

    /// Used by RenderPass to signal that a static format should be used
    NonSematic
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

#ifndef BLIB_RENDER_VULKAN_SAMPLER_HPP
#define BLIB_RENDER_VULKAN_SAMPLER_HPP

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Enum representing the different types of samplers available
 *
 * @ingroup Renderer
 */
enum struct Sampler {
    /// No filtering, clamped to border
    NoFilterBorderClamped,

    /// No filtering, clamped to edge
    NoFilterEdgeClamped,

    /// Minification filtering, clamped to border
    MinFilterBorderClamped,

    /// Magnification filtering, clamped to border
    MagFilterBorderClamped,

    /// Filtering, clamped to border
    FilteredBorderClamped,

    /// Filtering, repeated
    FilteredRepeated
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

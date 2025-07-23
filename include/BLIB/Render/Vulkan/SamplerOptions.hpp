#ifndef BLIB_RENDER_VULKAN_SAMPLEROPTIONS_HPP
#define BLIB_RENDER_VULKAN_SAMPLEROPTIONS_HPP

#include <cstddef>

namespace bl
{
namespace rc
{
namespace vk
{
/**
 * @brief Options used for creating sampler objects
 *
 * @ingroup Renderer
 */
struct SamplerOptions {
    /// Preset sampler types
    enum Type {
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

        /// Filtering, clamped to edge
        FilteredEdgeClamped,

        /// Filtering, repeated
        FilteredRepeated,

        // For shadow maps
        ShadowMap,

        /// The number of sampler types
        COUNT
    };

    static constexpr std::size_t TypeCount = static_cast<std::size_t>(Type::COUNT);

    Type type = Type::FilteredRepeated;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif

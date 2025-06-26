#ifndef BLIB_RENDER_CONFIG_LIMITS_HPP
#define BLIB_RENDER_CONFIG_LIMITS_HPP

#include <cstddef>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace cfg
{
/**
 * @brief Namespace containing limits for the renderer
 *
 * @ingroup Renderer
 */
struct Limits {
    static constexpr unsigned int MaxConcurrentFrames = 2;

    static constexpr unsigned int MaxSceneObservers = 64;

    static constexpr std::size_t MaxDescriptorSets = 4;

    static constexpr std::size_t MaxDescriptorBindings = 8;

    static constexpr std::uint32_t MaxRenderPasses = 8;

    static constexpr std::uint32_t MaxPipelineSpecializations = 16;

    static constexpr std::size_t MaxRenderPhases   = 16;
    static constexpr std::uint8_t MaxRenderPhaseId = MaxRenderPhases - 1;

    static constexpr std::uint32_t MaxTextureCount = 4096;

    static constexpr std::uint32_t MaxRenderTextures = 32;

    static constexpr std::uint32_t MaxCubemapCount = 16;

    static constexpr std::uint32_t MaxPointLights  = 128;
    static constexpr std::uint32_t MaxSpotLights   = 128;
    static constexpr std::uint32_t MaxPointShadows = 16;
    static constexpr std::uint32_t MaxSpotShadows  = 16;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif

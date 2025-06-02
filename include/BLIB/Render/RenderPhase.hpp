#ifndef BLIB_RENDER_RENDERER_RENDERPHASE_HPP
#define BLIB_RENDER_RENDERER_RENDERPHASE_HPP

#include <cstdint>
#include <type_traits>

#ifdef BLIB_WINDOWS
#include <intrin.h>
#endif

namespace bl
{
namespace rc
{
/**
 * @brief Enum representing the different possible phases of rendering
 *
 * @ingroup Renderer
 */
enum struct RenderPhase : std::uint8_t {
    /// Helper value representing no phases
    None = 0,

    /// The default phase. Represents rendering to the final swapchain output or intermediate post
    /// processing buffers
    Default = 0x1 << 0,

    /// Represents rendering to a shadow map
    ShadowMap = 0x1 << 1,

    /// Represents rendering to a point shadow map
    ShadowPointMap = 0x1 << 2,

    /// Represents rendering to UI overlay
    Overlay = 0x1 << 3,

    /// Represents post processing phases
    PostProcess = 0x1 << 4,

    /// Helper value representing all phases
    All = 0xFF
};

constexpr std::initializer_list<RenderPhase> AllRenderPhases{RenderPhase::Default,
                                                             RenderPhase::ShadowMap,
                                                             RenderPhase::ShadowPointMap,
                                                             RenderPhase::Overlay,
                                                             RenderPhase::PostProcess};

#ifdef BLIB_WINDOWS
/**
 * @brief Returns the index of the given render phase
 *
 * @param phase The single phase to get the index for
 * @return The 0 based index of the phase
 */
inline unsigned int renderPhaseIndex(RenderPhase phase) {
    unsigned long index = 0;
    if (phase != RenderPhase::None) {
        _BitScanForward(&index, static_cast<std::underlying_type_t<RenderPhase>>(phase));
    }
    return index;
}
#else
/**
 * @brief Returns the index of the given render phase
 *
 * @param phase The single phase to get the index for
 * @return The 0 based index of the phase
 */
inline unsigned int renderPhaseIndex(RenderPhase phase) {
    return phase != RenderPhase::None ?
               __builtin_ctz(static_cast<std::underlying_type_t<RenderPhase>>(phase)) :
               0;
}
#endif

} // namespace rc
} // namespace bl

/**
 * @brief Combine two render phases into one
 *
 * @param left The left render phase to combine
 * @param right The right render phase to combine
 * @return The combined render phase
 */
inline constexpr bl::rc::RenderPhase operator|(bl::rc::RenderPhase left,
                                               bl::rc::RenderPhase right) {
    return static_cast<bl::rc::RenderPhase>(
        static_cast<std::underlying_type_t<bl::rc::RenderPhase>>(left) |
        static_cast<std::underlying_type_t<bl::rc::RenderPhase>>(right));
}

/**
 * @brief Tests whether two phases overlap
 *
 * @param left The left phase to test
 * @param right The right phase to test
 * @return True if the phases overlap, false otherwise
 */
inline constexpr bool operator&(bl::rc::RenderPhase left, bl::rc::RenderPhase right) {
    return (static_cast<std::underlying_type_t<bl::rc::RenderPhase>>(left) &
            static_cast<std::underlying_type_t<bl::rc::RenderPhase>>(right)) != 0;
}

#endif

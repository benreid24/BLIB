#ifndef BLIB_RENDER_RENDERER_RENDERPHASE_HPP
#define BLIB_RENDER_RENDERER_RENDERPHASE_HPP

#include <cstdint>
#include <type_traits>

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

    /// Represents rendering to an offscreen texture
    OffscreenTexture = 0x1 << 1,

    /// Represents rendering to a shadow map
    ShadowMap = 0x1 << 2,

    /// Helper value representing all phases
    All = 0xFF
};

/**
 * @brief Combine two render phases into one
 *
 * @param left The left render phase to combine
 * @param right The right render phase to combine
 * @return The combined render phase
 */
inline RenderPhase operator|(RenderPhase left, RenderPhase right) {
    return static_cast<RenderPhase>(static_cast<std::underlying_type_t<RenderPhase>>(left) |
                                    static_cast<std::underlying_type_t<RenderPhase>>(right));
}

/**
 * @brief Tests whether two phases overlap
 *
 * @param left The left phase to test
 * @param right The right phase to test
 * @return True if the phases overlap, false otherwise
 */
inline bool operator&(RenderPhase left, RenderPhase right) {
    return (static_cast<std::underlying_type_t<RenderPhase>>(left) &
            static_cast<std::underlying_type_t<RenderPhase>>(right)) != 0;
}

} // namespace rc
} // namespace bl

#endif

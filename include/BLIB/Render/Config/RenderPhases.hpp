#ifndef BLIB_RENDER_CONFIG_RENDERPHASES_HPP
#define BLIB_RENDER_CONFIG_RENDERPHASES_HPP

#include <BLIB/Render/RenderPhase.hpp>

namespace bl
{
namespace rc
{
namespace cfg
{

/**
 * @brief Collection of built-in render phases
 *
 * @ingroup Renderer
 */
struct RenderPhases {
    /// The default forward render phase
    static constexpr RenderPhase Forward = 0;

    /// Use when doing deferred rendering
    static constexpr RenderPhase Deferred = 1;

    /// Use when rendering to a shadow map
    static constexpr RenderPhase ShadowMap = 2;

    /// Use when rendering to a point shadow map
    static constexpr RenderPhase ShadowPointMap = 3;

    /// Use when rendering to UI overlay
    static constexpr RenderPhase Overlay = 4;

    /// Use when post processing phases
    static constexpr RenderPhase PostProcess = 5;

    /// Use when rendering an object outline
    static constexpr RenderPhase Outline = 6;
};

} // namespace cfg
} // namespace rc
} // namespace bl

#endif

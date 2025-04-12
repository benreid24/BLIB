#ifndef BLIB_RENDER_GRAPH_ASSETS_RENDERPASSBEHAVIOR_HPP
#define BLIB_RENDER_GRAPH_ASSETS_RENDERPASSBEHAVIOR_HPP

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Represents when render passes are started in the render graph
 *
 * @ingroup Renderer
 */
enum struct RenderPassBehavior {
    /// The render pass is started by the asset
    StartedByAsset,

    /// The render pass is started by the task
    StartedByTask,

    /// The render pass is started outside of the graph
    StartedExternally
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

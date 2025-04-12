#ifndef BLIB_RENDER_GRAPH_STRATEGIES_OVERLAYRENDERSTRATEGY_HPP
#define BLIB_RENDER_GRAPH_STRATEGIES_OVERLAYRENDERSTRATEGY_HPP

#include <BLIB/Render/Graph/Strategy.hpp>

namespace bl
{
namespace rc
{
/// Contains render graph asset, task, and strategy implementations
namespace rgi
{
/**
 * @brief Render strategy for Overlay scenes. Simply adds the RenderOverlayTask
 *
 * @ingroup Renderer
 */
class OverlayRenderStrategy : public rg::Strategy {
public:
    /**
     * @brief Destroys the strategy
     */
    virtual ~OverlayRenderStrategy() = default;

    /**
     * @brief Adds the rendering tasks to the given graph
     *
     * @param graph The graph to add tasks to
     */
    virtual void populate(rg::RenderGraph& graph) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

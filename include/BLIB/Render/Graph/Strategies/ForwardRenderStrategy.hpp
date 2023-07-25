#ifndef BLIB_RENDER_GRAPH_STRATEGIES_FORWARDRENDERSTRATEGY_HPP
#define BLIB_RENDER_GRAPH_STRATEGIES_FORWARDRENDERSTRATEGY_HPP

#include <BLIB/Render/Graph/Strategy.hpp>

namespace bl
{
namespace rc
{
/// Contains render graph asset, task, and strategy implementations
namespace rgi
{
/**
 * @brief Default render strategy. Implements a basic forward renderer
 *
 * @ingroup Renderer
 */
class ForwardRenderStrategy : public rg::Strategy {
public:
    /**
     * @brief Destroys the strategy
     */
    virtual ~ForwardRenderStrategy() = default;

    /**
     * @brief Adds the forward rendering task to the given graph
     *
     * @param graph The graph to add the task to
     */
    virtual void populate(rg::RenderGraph& graph) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

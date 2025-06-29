#ifndef BLIB_RENDER_GRAPH_STRATEGIES_SCENE3DDEFERREDRENDERSTRATEGY_HPP
#define BLIB_RENDER_GRAPH_STRATEGIES_SCENE3DDEFERREDRENDERSTRATEGY_HPP

#include <BLIB/Render/Graph/Strategy.hpp>

namespace bl
{
namespace rc
{
/// Contains render graph asset, task, and strategy implementations
namespace rgi
{
/**
 * @brief Deferred render strategy for 3d scenes
 *
 * @ingroup Renderer
 */
class Scene3DDeferredRenderStrategy : public rg::Strategy {
public:
    /**
     * @brief Destroys the strategy
     */
    virtual ~Scene3DDeferredRenderStrategy() = default;

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

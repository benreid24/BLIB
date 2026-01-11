#ifndef BLIB_RENDER_GRAPH_STRATEGIES_SCENE3DFORWARDRENDERSTRATEGY_HPP
#define BLIB_RENDER_GRAPH_STRATEGIES_SCENE3DFORWARDRENDERSTRATEGY_HPP

#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Graph/Strategy.hpp>
#include <BLIB/Signals/Listener.hpp>

namespace bl
{
namespace rc
{
/// Contains render graph asset, task, and strategy implementations
namespace rgi
{
/**
 * @brief Forward render strategy for 3d scenes
 *
 * @ingroup Renderer
 */
class Scene3DForwardRenderStrategy
: public rg::Strategy
, public sig::Listener<event::SettingsChanged> {
public:
    /**
     * @brief Creates the strategy
     */
    Scene3DForwardRenderStrategy();

    /**
     * @brief Destroys the strategy
     */
    virtual ~Scene3DForwardRenderStrategy() = default;

    /**
     * @brief Adds the rendering tasks to the given graph
     *
     * @param graph The graph to add tasks to
     */
    virtual void populate(rg::RenderGraph& graph) override;

    /**
     * @brief Must call at least once before use
     *
     * @param renderer The renderer instance
     */
    void init(Renderer& renderer);

private:
    Renderer* renderer;

    virtual void process(const event::SettingsChanged& signal) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif

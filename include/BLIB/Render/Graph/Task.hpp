#ifndef BLIB_RENDER_GRAPH_TASK_HPP
#define BLIB_RENDER_GRAPH_TASK_HPP

#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/Graph/TaskAssetTags.hpp>
#include <string_view>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
class GraphAsset;
class RenderGraph;

/**
 * @brief Base class for tasks within a RenderGraph
 *
 * @ingroup Renderer
 */
class Task {
public:
    /**
     * @brief Destroys the task
     */
    virtual ~Task() = default;

    /**
     * @brief Called once after the task is created within a RenderGraph
     *
     * @param engine The engine instance
     * @param renderer The renderer instance
     */
    virtual void create(engine::Engine& engine, Renderer& renderer) = 0;

    /**
     * @brief Called after assets are assigned and created. Called when graph is rebuilt
     *
     * @param inputs The inputs to this task
     * @param output The output to this task
     */
    virtual void onGraphInit(const std::vector<GraphAsset*>& inputs, GraphAsset* output) = 0;

    /**
     * @brief Called once per rendered frame
     *
     * @param ctx The execution context
     */
    virtual void execute(ExecutionContext& ctx) = 0;

protected:
    TaskAssetTags assetTags;

    /**
     * @brief Initializes the task
     */
    Task();

private:
    std::vector<GraphAsset*> inputs;
    GraphAsset* output;

    // Called by RenderGraph
    void reset();

    friend class RenderGraph;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif

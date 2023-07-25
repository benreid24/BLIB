#ifndef BLIB_RENDER_GRAPH_TASK_HPP
#define BLIB_RENDER_GRAPH_TASK_HPP

#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/Graph/GraphAsset.hpp>
#include <BLIB/Render/Graph/TaskAssetTags.hpp>
#include <BLIB/Render/Graph/TaskAssets.hpp>
#include <string_view>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
struct GraphAsset;
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
     */
    virtual void onGraphInit() = 0;

    /**
     * @brief Called once per rendered frame
     *
     * @param ctx The execution context
     */
    virtual void execute(const ExecutionContext& ctx) = 0;

    /**
     * @brief Called each frame with the elapsed time
     *
     * @param dt The elapsed time in seconds since the last call to update
     */
    virtual void update(float dt);

protected:
    TaskAssetTags assetTags;
    TaskAssets assets;

private:
    friend class RenderGraph;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif

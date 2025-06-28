#ifndef BLIB_RENDER_GRAPH_TASK_HPP
#define BLIB_RENDER_GRAPH_TASK_HPP

#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/Graph/GraphAsset.hpp>
#include <BLIB/Render/Graph/TaskAssetTags.hpp>
#include <BLIB/Render/Graph/TaskAssets.hpp>
#include <limits>
#include <string_view>
#include <vector>

namespace bl
{
namespace rc
{
class Scene;

namespace rg
{
struct GraphAsset;
class RenderGraph;
class Timeline;

/**
 * @brief Base class for tasks within a RenderGraph
 *
 * @ingroup Renderer
 */
class Task {
public:
    /**
     * @brief Initializes the task
     *
     * @param id The id of the task
     */
    Task(std::string_view id);

    /**
     * @brief Destroys the task
     */
    virtual ~Task() = default;

    /**
     * @brief Called once after the task is created within a RenderGraph
     *
     * @param engine The engine instance
     * @param renderer The renderer instance
     * @param scene The scene the graph is rendering to
     */
    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) = 0;

    /**
     * @brief Called after assets are assigned and created. Called when graph is rebuilt
     */
    virtual void onGraphInit() = 0;

    /**
     * @brief Called once per output. If a task has multiple outputs this will be called multiple
     *        times. Should only operate on the current output
     *
     * @param ctx The execution context
     * @param output The asset being executed on
     */
    virtual void execute(const ExecutionContext& ctx, Asset* output) = 0;

    /**
     * @brief Called each frame with the elapsed time
     *
     * @param dt The elapsed time in seconds since the last call to update
     */
    virtual void update(float dt);

    /**
     * @brief Returns the id of the task
     */
    std::string_view getId() const { return id; }

protected:
    TaskAssetTags assetTags;
    TaskAssets assets;

private:
    std::string_view id;

    void prepareInputs(const ExecutionContext& ctx);

    friend class RenderGraph;
    friend class Timeline;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif

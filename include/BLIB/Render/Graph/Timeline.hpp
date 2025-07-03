#ifndef BLIB_RENDER_GRAPH_TIMELINE_HPP
#define BLIB_RENDER_GRAPH_TIMELINE_HPP

#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/Graph/TaskOutput.hpp>
#include <BLIB/Vulkan.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}
namespace rc
{
class Renderer;
class RenderTarget;
class Scene;

namespace rg
{
class Task;
class Asset;
struct GraphAsset;
class GraphAssetPool;

/**
 * @brief Execution timeline for the render graph
 *
 * @ingroup Renderer
 */
class Timeline {
public:
    /**
     * @brief A group of assets that must run in sequence and output to the same asset
     *
     * @ingroup Renderer
     */
    struct TaskGroup {
        std::vector<std::pair<Task*, TaskOutput::Order>> tasks;
        GraphAsset* output;

        /**
         * @brief Creates an empty task group
         *
         * @param output The asset being created by the group
         */
        TaskGroup(GraphAsset* output);

        /**
         * @brief Move constructor
         */
        TaskGroup(TaskGroup&&) = default;

        /**
         * @brief Adds the task to this group in the appropriate order
         *
         * @param task The task to add
         * @param order The order the task should be in
         */
        void addTask(Task* task, TaskOutput::Order order);

        /**
         * @brief Executes the tasks in this group
         *
         * @param ctx The graph execution context
         */
        void execute(const ExecutionContext& ctx);
    };

    /**
     * @brief A single stage in the timeline comprised of task groups that run together
     *
     * @ingroup Renderer
     */
    struct TimelineStage {
        std::vector<TaskGroup> taskGroups;

        /**
         * @brief Creates an empty timeline stage
         */
        TimelineStage();

        /**
         * @brief Move constructor
         */
        TimelineStage(TimelineStage&&) = default;

        /**
         * @brief Gets or creates the task group for an asset
         *
         * @param asset The asset being created
         * @return The task group for the asset
         */
        TaskGroup* getGroupForAsset(GraphAsset* asset);

        /**
         * @brief Executes the groups in this pipeline stage
         *
         * @param ctx The graph execution context
         */
        void execute(const ExecutionContext& ctx);
    };

    /**
     * @brief Creates an empty timeline
     *
     * @param engine The game engine instance
     * @param renderer The renderer instance
     * @param target The render target of the graph
     * @param scene The scene the timeline belongs to
     * @param pool The asset pool of the graph that owns this timeline
     */
    Timeline(engine::Engine& engine, Renderer& renderer, RenderTarget* target, Scene* scene,
             GraphAssetPool* pool);

    /**
     * @brief Builds the timeline from the given final asset. Assets must have already been
     *        traversed and populated by RenderGraph
     *
     * @param tasks All of the tasks that comprise the timeline
     * @param finalAsset The final output asset. Used for validation
     */
    void build(std::vector<std::unique_ptr<Task>>& tasks, GraphAsset* finalAsset);

    /**
     * @brief Executes the timeline to record render commands into the given command buffer
     *
     * @param ctx The graph execution context
     */
    void execute(const ExecutionContext& ctx);

private:
    engine::Engine& engine;
    Renderer& renderer;
    RenderTarget* observer;
    Scene* scene;
    GraphAssetPool* pool;
    std::vector<TimelineStage> timeline;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
